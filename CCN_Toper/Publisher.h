/*
 * Publisher.h
 *
 *  Created on: Mar 20, 2016
 *      Author: njnu
 */

#ifndef PUBLISHER_H_
#define PUBLISHER_H_

#include <json/json.h>
#include <json/reader.h>
#include <json/value.h>
#include <vector>

#include "../CCN_Layer/DataPanel.h"
#include "PublishThreading.h"

class Publisher: public DataPanel {
public:
	Publisher(std::string localIP);
	virtual ~Publisher();

	/*
	 * function: publish the content according the time
	 */
	void publish();


private:

	/*
	 * function: read the content to publish from the file
	 * parameters: the name of the file which including the content to publish
	 */
	void readContent(string fileName);

	/*
	 * function: publish the content with its content description
	 * parameters: content : the content to send
	 *             cds : the content description of the content to publish
	 */
	void publishContent(string content, string cds);

	/*
	 * function:send message
	 * parameter: cds:the description of the content
	 * 					content
	 * 					rendezvous
	 */
	void publishSendMsg(std::string contentName, std::string content,
			std::string rendezvous);

private:
	Json::Value m_contentSet; //the set of the content to publish which including the content and cd and sending time

	PublishThreading m_publishThreading;

	int m_sequenceNumber;

	bool m_isSubListNew;

};

#endif /* PUBLISHER_H_ */
