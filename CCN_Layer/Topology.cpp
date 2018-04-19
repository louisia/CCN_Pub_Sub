/*
 * Topology.cpp
 *
 *  Created on: Sep 14, 2015
 *      Author: njnu
 */

#include "Topology.h"

#include <fstream>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <algorithm>

Topology::Topology() {
    // TODO Auto-generated constructor stub

}

Topology::~Topology() {
    // TODO Auto-generated destructor stub
}

/*
 * function: init the topology from the file
 */
void Topology::initFromFile( )
{
    //open the file
    std::ifstream ifs;

    ifs.open( "../Files/topology.txt" );

    if( !ifs )
    {
        std::cerr<<"the topology file does not exist..."<<std::endl;

        exit( 1 );
    }else{
        std::cout<<"the topology file open succeed..."<<std::endl;

    }

    //the buffer to store the line of the file
    char buffer[ 512 ];

    //the roleID of the node in the link
    RoleID roleIDs[ 2 ];

    //analysis each line of the file
    while( !ifs.eof() )
    {
        memset( buffer, 0, sizeof( buffer ) );

        ifs.getline( buffer, sizeof( buffer ) );

        const int len = strlen( buffer );

        //ignore the empty line and the comment line and the special line
        if( 0 == len || '#' == buffer[ 0 ] || '*' == buffer[ len - 1 ] )
            continue ;

        //*/debug
        std::cout<<"line: "<<buffer<<std::endl;
        //*/

        //translate the buffer into roleID of the node in the link
        int posstart = 0, posend = 0;
        for( int i = 0; i < 2; ++i )
        {
            posstart = posend;

            while( posstart < len && ( buffer[ posstart ] < 'a' || buffer[ posstart ] > 'z' ) )
                posstart++;

            if( posstart >= len )
            {
                std::cerr<<"error link occur..."<<std::endl;

                //*/debug
                std::cout<<"error posstart: "<<posstart<<" posend: "<<posend<<std::endl;
                //*/

                exit( 1 );
            }

            posend = posstart;

            while( posend < len && ' ' != buffer[ posend ] && '-' != buffer[ posend ] )
                posend++;


            roleIDs[ i ] = translateToRoleID( buffer + posstart, posend - posstart );
        }

        //add the link read
        addLink( roleIDs[ 0 ], roleIDs[ 1 ] );
    }

}

/*
 * function: add a link into the topology
 * parameters:rid1: the roleID of the node of the link to add
 *            rid2: the roleID of the another node of the link to add
 */
void Topology::addLink( RoleID rid1, RoleID rid2 )
{
    m_nodeSet[ addNode( rid1 ) ].addAdjacent( rid2 );

    m_nodeSet[ addNode( rid2 ) ].addAdjacent( rid1 );
}

/*
 * function: add a node into the topology
 * parameters: roleID: the roleID of the node to add
 * return value: the index of the node added
 */
int Topology::addNode( RoleID roleID )
{
    //find the named node
    int index = findNode( roleID );
    if( -1 != index )
        return index;

    TopologyNode node;
    node.setRoleID( roleID );

    //add the node
    m_nodeSet.push_back( node );

    return m_nodeSet.size() - 1;
}

/*
 * function: find the named node with the roldID
 * parameters: roleID: the roleID of the node to find
 * return value: return the index of the node if found, otherwise return -1
 */
int Topology::findNode( RoleID roleID )
{
    for( std::vector< TopologyNode >::size_type i = 0; i != m_nodeSet.size(); ++i )
    {
        RoleID rid = m_nodeSet[ i ].getRoleID();
        if( rid._role == roleID._role && rid._id == roleID._id )
            return i;
    }

    return -1;
}

/*
 * function: translate the buffer into the roleID
 * parameters: buffer: the information of the roleID
 *             len: the length of the buffer
 * return value: the roleID translated
 */
RoleID Topology::translateToRoleID( char* buffer, int len )
{
    RoleID roleID;

    int pos = 0;

    //find the range of the role identification
    while( pos < len && buffer[ pos ] >= 'a' && buffer[ pos ] <= 'z' )
        pos++;


    char* nodeTypes[] = { "ctl", "rot",  "sub", "rrot", "pub","erot" };

    for( int i = 0; i < sizeof( nodeTypes ) / sizeof( char* ); ++i )
    {
        if( 0 == strncmp( buffer, nodeTypes[ i ], pos ) )
        {
            roleID._role = (Role)i;

            break;
        }
    }

    //determine the id of the node
    if( pos >= len )
    {
        roleID._id = 0;
    }
    else
    {
        roleID._id = atoi( std::string( buffer+pos, len - pos ).c_str() );
    }

    //*/debug
    std::cout<<"role: "<<nodeTypes[ roleID._role ]<<" id: "<<roleID._id<<std::endl;
    //*/
    return roleID;
}

/*
 * function: get the router table of the node with roleID
 * parameters: roleID: the roleID of the node to get router table
 * return value: the router table of the node with roleID
 */
std::vector< RouterItem > Topology::getRouterTable( RoleID roleID )
{
    //find the index of the roleID in the nodeSet
    int source = findNode( roleID );
    if( -1 == source )
    {
        std::cerr<<"the roleID does not exist..."<<std::endl;
        return std::vector< RouterItem >();
    }

    //get the pre-node table for the roleID
    std::vector< std::vector< int > > preTable = multiPathDijkstra( source );

    //analysis the pre-node table to get the router table
    std::vector< RouterItem > routerTable;

    for( std::vector< std::vector< int > >::size_type i = 0; i != preTable.size(); ++i )
    {
        RouterItem item;

        //set the destination
        item._des = m_nodeSet[ i ].getRoleID();

        std::vector< int > nextIndex = translatePreToNextTable( i, preTable, source );

        for( std::vector< int >::size_type j = 0; j != nextIndex.size(); ++j )
        {
            item._nextSet.push_back( m_nodeSet[ nextIndex[ j ] ].getRoleID() );
        }

        routerTable.push_back( item );
    }

    sleep( 1 );

    return routerTable;
}

/*
 * function: translate the pre-node into the next-node table
 * parameters: index: the index of the preTable to translate
 *             preTable: the pre-node table to translate
 *             source: the source of the path
 * return value: the next-node table translate
 */
std::vector< int > Topology::translatePreToNextTable( int index, std::vector< std::vector< int > > preTable, int source )
{
    if( index == source )
        return std::vector< int >();

    std::vector< int > nextIndex, temp;

    //the current index of the node to translate is the next index of the hop
    nextIndex.push_back( index );

    //set the next hop to go to the destination
    bool isContinue = true;
    while( isContinue )
    {
        //update the nextIndex
        for( std::vector< int >::size_type j = 0; j != nextIndex.size(); ++ j )
        {
            //check the pre-node of the nextIndex
            if( preTable[ nextIndex[ j ] ].size() != 0 && preTable[ nextIndex[ j ] ][ 0 ] != source )
            {
                temp.insert( temp.end(), preTable[ nextIndex[ j ] ].begin(), preTable[ nextIndex[ j ] ].end() );
            }
            else //if( preTable[ nextIndex[ j ] ].size() != 0 )//preTable[ nextIndex[ j ] ][ 0 ] == source
            {
                temp.push_back( nextIndex[ j ] );
            }
        }

        isContinue = false;

        //check the pre-node
        for( std::vector< int >::size_type j = 0; j != temp.size(); ++ j )
        {
            if( preTable[ temp[ j ] ].size() != 0 && preTable[ temp[ j ] ][ 0 ] != source )
            {
                isContinue = true;
            }
        }

        nextIndex = temp;

        //clean the temp
        temp.resize( 0 );
    }

    //delete the copy index
    std::sort( nextIndex.begin(), nextIndex.end() );
    std::vector< int >::iterator iter = unique( nextIndex.begin(), nextIndex.end() );
    nextIndex.erase( iter, nextIndex.end() );


    return nextIndex;
}

/*
 * function: using the dijkstra to find the multiple shortest path
 * parameters: index: the index of the source node in the nodeSet
 * return value: the pre-node of each destination
 */
std::vector< std::vector< int > > Topology::multiPathDijkstra( int index )
{
    if( -1 == index )
        return std::vector< std::vector< int > >();

    int maxCount = m_nodeSet.size();
    std::vector< std::vector< int > > preTable( maxCount );
    std::vector< int > dist( maxCount, MAXHOP );
    std::vector< bool > isChoiced( maxCount, false );

    int count = 0;

    //set the source
    dist[ index ] = 0;
    isChoiced[ index ] = true;
    count++;

    while( count < maxCount )
    {
        //update
        //here can using the adjacent information to update
        std::vector< RoleID >& adjacent = m_nodeSet[ index ].getAdjacent();
        for( std::vector< RoleID >::size_type i = 0; i != adjacent.size(); ++i )
        {
            int adIndex = findNode( adjacent[ i ] );

            if( isChoiced[ adIndex ] )
                continue;

            if( dist[ adIndex ] > dist[ index ] + 1 )
            {
                dist[ adIndex ] = dist[ index ] + 1;

                //reset the pre-node table
                preTable[ adIndex ].resize( 1, index );
            }
            else if( dist[ adIndex ] == dist[ index ] + 1 )
            {
                //add the pre-node
                preTable[ adIndex ].push_back( index );
            }
        }


        //choice
        int minDist = MAXHOP;
        for( std::vector< int >::size_type i = 0; i != dist.size(); ++i )
        {
            if( isChoiced[ i ] )
                continue;

            if( dist[ i ] <= minDist )
            {
                index = i;
                minDist = dist[ i ];
            }
        }

        isChoiced[ index ] = true;


        count++;
    }


    return preTable;
}

/*
 * function: apply the node for the role
 * parameters: comAddr: the communicate information set
 * return value: the id of the role applied
 */
int Topology::applyNode( Role role, CommunicateAddr comAddr )
{
    for( std::vector< TopologyNode >::size_type i = 0; i != m_nodeSet.size(); ++i )
    {
        if( m_nodeSet[ i ].getAssigned() || m_nodeSet[ i ].getRoleID()._role != role )
            continue;

        m_nodeSet[ i ].setCommunicateAddr( comAddr );
        m_nodeSet[ i ].setAssigned( true );

        return m_nodeSet[ i ].getRoleID()._id;
    }

    return -1;
}

/*
 * function: get the number of node in the topology
 * return value: the number of node
 */
int Topology::getNodeNum()
{
    return m_nodeSet.size();
}

/*
 * function: get node by index
 * parameters: index: the index of the node to find
 * return value: the node found
 */
TopologyNode Topology::getNodeByIndex( int index )
{
    if( index >= m_nodeSet.size() || index < 0 )
    {
        std::cerr<<"error index: "<<index<<std::endl;

        exit( 1 );
    }

    return m_nodeSet[ index ];
}

std::ostream& operator<<( std::ostream& ofs, Topology& top )
{
    for( std::vector< TopologyNode >::size_type i = 0; i != top.m_nodeSet.size(); ++i )
    {
        std::cout<<top.m_nodeSet[ i ]<<std::endl;
    }
    return ofs;
}
