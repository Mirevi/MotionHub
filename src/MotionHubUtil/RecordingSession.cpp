#include "RecordingSession.h"




#pragma region Session

RecordingSession::RecordingSession()
{

}

void RecordingSession::addFrame(RecordingFrame frame, float duration)
{
	if (m_frames.size() > 0)
	{
		m_frames.back().m_duration = duration;
	}


	m_frames.push_back(frame);
}

void RecordingSession::save()
{

	tinyxml2::XMLDocument doc;
	tinyxml2::XMLNode* pRoot = doc.NewElement("Frames");
	
	doc.InsertFirstChild(pRoot);

	int iF = 0;


	//loop through all frames
	for (auto itFrames = m_frames.begin(); itFrames != m_frames.end(); itFrames++)
	{

		std::string str = "Frame_" + std::to_string(iF++);
		const char* chr = str.c_str();

		tinyxml2::XMLNode* pFrame = doc.NewElement(chr);


		int iS = 0;

		//loop through all skeletons of a frame
		for (auto itSkeleton = itFrames->m_skeletons.begin(); itSkeleton != itFrames->m_skeletons.end(); itSkeleton++)
		{

			std::string strs = "Skeleton_" + std::to_string(iS++);
			const char* chrs = strs.c_str();

			tinyxml2::XMLElement* pSkeleton = doc.NewElement(chrs);

			//loop through joints
			for (auto itJoint = itSkeleton->m_joints.begin(); itJoint != itSkeleton->m_joints.end(); itJoint++)
			{

				std::string strj = "Joint_" + Joint::toString(itJoint->first);
				const char* chrj = strj.c_str();




				tinyxml2::XMLElement* pJoint = doc.NewElement(chrj);

				

				//insert position
				tinyxml2::XMLElement* pPosition = doc.NewElement("position");

				tinyxml2::XMLElement* pPosX = doc.NewElement("x");
				tinyxml2::XMLElement* pPosY = doc.NewElement("y");
				tinyxml2::XMLElement* pPosZ = doc.NewElement("z");

				pPosX->SetText(std::to_string(itJoint->second.getJointPosition().x()).c_str());
				pPosY->SetText(std::to_string(itJoint->second.getJointPosition().y()).c_str());
				pPosZ->SetText(std::to_string(itJoint->second.getJointPosition().z()).c_str());

				pPosition->InsertEndChild(pPosX);
				pPosition->InsertEndChild(pPosY);
				pPosition->InsertEndChild(pPosZ);

				pJoint->InsertEndChild(pPosition);

				//insert rotation
				tinyxml2::XMLElement* pRotation = doc.NewElement("rotation");

				tinyxml2::XMLElement* pRotX = doc.NewElement("x");
				tinyxml2::XMLElement* pRotY = doc.NewElement("y");
				tinyxml2::XMLElement* pRotZ = doc.NewElement("z");
				tinyxml2::XMLElement* pRotW = doc.NewElement("w");


				pRotX->SetText(std::to_string(itJoint->second.getJointRotation().x()).c_str());
				pRotY->SetText(std::to_string(itJoint->second.getJointRotation().y()).c_str());
				pRotZ->SetText(std::to_string(itJoint->second.getJointRotation().z()).c_str());
				pRotW->SetText(std::to_string(itJoint->second.getJointRotation().w()).c_str());

				pRotation->InsertEndChild(pRotX);
				pRotation->InsertEndChild(pRotY);
				pRotation->InsertEndChild(pRotZ);
				pRotation->InsertEndChild(pRotW);


				pJoint->InsertEndChild(pRotation);



				//insert Confidence
				tinyxml2::XMLElement* pConfidence = doc.NewElement("confidence");
				pConfidence->SetText(std::to_string(itJoint->second.getJointConfidence()).c_str());
				pJoint->InsertEndChild(pConfidence);




				pSkeleton->InsertEndChild(pJoint);

			}



			pFrame->InsertEndChild(pSkeleton);

		}

		//insert frameTime
		tinyxml2::XMLElement* pFrameTime = doc.NewElement("frametime");
		pFrameTime->SetText(std::to_string(itFrames->m_duration).c_str());
		pFrame->InsertEndChild(pFrameTime);

		pRoot->InsertEndChild(pFrame);

	}





	std::string filename = "MMH_" + Timer::getCurrTime() + ".mmh";
	std::string pathStr = RECORD_PATH;
	filename = pathStr + filename;

	filename = removeChar(filename, ' ');
	filename = removeChar(filename, ':');

	tinyxml2::XMLError error = doc.SaveFile(filename.c_str());



	if ((int)error == 0)
	{
		Console::log("RecordingSession::save(): Session saved as: " + filename);
	}
	else
	{
		Console::logError("RecordingSession::save(): Error code " + std::to_string((int)error));
	}


}

void RecordingSession::load(std::string filePath)
{
	tinyxml2::XMLDocument doc;

	int res = doc.LoadFile(filePath.c_str());

	if (res == 0)
	{
		Console::log("RecordingSession::load(): file read successfully. Start loading data");
	}
	else
	{
		Console::logError("RecordingSession::load(): Error loading file. tinyxml Error code " + toString(res));
	}


	



	tinyxml2::XMLElement const* rootElement = doc.RootElement();

	//Loop over all frames
	for (tinyxml2::XMLElement const* itFrame = rootElement->FirstChildElement(); itFrame != NULL; itFrame = itFrame->NextSiblingElement())
	{
		
			RecordingFrame currFrameObj = RecordingFrame();
			float duration;
			
		//Loop over all skeletons or frametimes
		for (tinyxml2::XMLElement const* itSkeleton = itFrame->FirstChildElement(); itSkeleton != NULL; itSkeleton = itSkeleton->NextSiblingElement())
		{

			std::string currName(itSkeleton->Name());


			//Console::log("RecordingSession::load(): " + std::string(itSkeleton->Name()) + ", rfind: " + toString(currName.rfind("Skeleton_", 0)));

			//check if it's a skeleton or the frame time
			if (strcmp(itSkeleton->Name(), "frametime") == 0)
			{
				duration = std::stof(itSkeleton->GetText());
			}
			else if (currName.rfind("Skeleton_", 0) == 0)
			{
				currName.erase(0, 9);

				Skeleton currSkeleton = Skeleton(std::stoi(currName));				

				//Console::log("RecordingSession::load(): " + currName);

				int jointIdx = 0;

				//loop over all joints
				for (tinyxml2::XMLElement const* itJoint = itSkeleton->FirstChildElement(); itJoint != NULL; itJoint = itJoint->NextSiblingElement())
				{

					Vector4f position;
					Quaternionf rotation;
					Joint::JointConfidence confidence;

					Joint::JointNames currType = Joint::toJointNames(std::string(itJoint->Name()).erase(0, 6));

					//Console::log("RecordingSession::save(): current type: " + Joint::toString(currType));


					//loop over position, rotation and confidence
					for (tinyxml2::XMLElement const* itJointElement = itJoint->FirstChildElement(); itJointElement != NULL; itJointElement = itJointElement->NextSiblingElement())
					{

						//check if it's position, rotation or confidence
						if (strcmp(itJointElement->Name(), "position") == 0)
						{

							float x;
							float y;
							float z;

							//loop over x, y and z
							for (tinyxml2::XMLElement const* itPositionElement = itJointElement->FirstChildElement(); itPositionElement != NULL; itPositionElement = itPositionElement->NextSiblingElement())
							{


								if (strcmp(itPositionElement->Name(), "x") == 0)
								{
									x = std::atof(itPositionElement->GetText());
								}
								else if (strcmp(itPositionElement->Name(), "y") == 0)
								{
									y = std::atof(itPositionElement->GetText());
								}
								else if (strcmp(itPositionElement->Name(), "z") == 0)
								{
									z = std::atof(itPositionElement->GetText());
								}

							}

							position = Vector4f(x, y, z, 0.0f);

							//Console::log("RecordingSession::load(): position: " + toString(position));

						}
						else if (strcmp(itJointElement->Name(), "rotation") == 0)
						{

							float x;
							float y;
							float z;
							float w;

							//loop over x, y, z and w
							for (tinyxml2::XMLElement const* itRotationElement = itJointElement->FirstChildElement(); itRotationElement != NULL; itRotationElement = itRotationElement->NextSiblingElement())
							{


								if (strcmp(itRotationElement->Name(), "x") == 0)
								{
									x = std::atof(itRotationElement->GetText());
								}
								else if (strcmp(itRotationElement->Name(), "y") == 0)
								{
									y = std::atof(itRotationElement->GetText());
								}
								else if (strcmp(itRotationElement->Name(), "z") == 0)
								{
									z = std::atof(itRotationElement->GetText());
								}
								else if (strcmp(itRotationElement->Name(), "w") == 0)
								{
									w = std::atof(itRotationElement->GetText());

								}



							}

							rotation = Quaternionf(x, y, z, w);
							//Console::log("RecordingSession::load(): rotation: " + toString(rotation));

						}
						else if (strcmp(itJointElement->Name(), "confidence") == 0)
						{
							//assign confidence
							confidence = (Joint::JointConfidence)std::atoi(itJointElement->GetText());
							//Console::log("RecordingSession::load(): confidence: " + toString(confidence));


						}

					}

					//add joint to pool
					currSkeleton.m_joints[currType] = Joint(position, rotation, confidence);

				}

				//Console::log("RecordingSession::load(): joint count: " + toString(currSkeleton.m_joints.size()));
				currFrameObj.addSkeleton(currSkeleton);

			}

		}
		
		addFrame(currFrameObj, duration);
	
		//Console::log("RecordingSession::load(): " + std::string(itFrame->Name()) + ", skeleton count = " + toString(currFrameObj.m_skeletons.size()));
	}

	Console::log("RecordingSession::load(): done loading data.");

}

int RecordingSession::getFrameCount()
{
	return m_frames.size();
}

RecordingFrame* RecordingSession::getFrame(int i)
{
	return &m_frames[i];
}



#pragma endregion



#pragma region Frame

RecordingFrame::RecordingFrame()
{



}



void RecordingFrame::addSkeleton(Skeleton currSkeleton)
{
	m_skeletons.push_back(currSkeleton);
}


#pragma endregion
