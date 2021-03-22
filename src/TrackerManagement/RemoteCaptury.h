#pragma once

#include <stdint.h>
#include "captury/PublicStructs.h"

#ifdef WIN32
#define CAPTURY_DLL_EXPORT __declspec(dllexport)
#else
#define CAPTURY_DLL_EXPORT
#endif

#ifdef __cplusplus
extern "C" {
#endif




// returns 1 if successful, 0 otherwise
// the default port is 2101
CAPTURY_DLL_EXPORT int Captury_connect(const char* ip, unsigned short port);
// in case you need to set the local port because of firewalls, etc.
// use 0 for localPort and localStreamPort if you don't care
CAPTURY_DLL_EXPORT int Captury_connect2(const char* ip, unsigned short port, unsigned short localPort, unsigned short localStreamPort);

// returns 1 if successful, 0 otherwise
CAPTURY_DLL_EXPORT int Captury_disconnect();




// returns the number of actors
// on exit *cameras points to an array of CapturyCamera
// the array is owned by the library - do not free
CAPTURY_DLL_EXPORT int Captury_getActors(const CapturyActor** actors);

// returns the actor or NULL if it is not known
// the struct is owned by the library - do not free
CAPTURY_DLL_EXPORT const CapturyActor* Captury_getActor(int actorId);

// returns the number of cameras
// on exit *cameras points to an array of CapturyCamera
// the array is owned by the library - do not free
CAPTURY_DLL_EXPORT int Captury_getCameras(const CapturyCamera** cameras);




#define CAPTURY_STREAM_NOTHING		0x00
#define CAPTURY_STREAM_POSES		0x01
#define CAPTURY_STREAM_GLOBAL_POSES	0x01
#define CAPTURY_STREAM_LOCAL_POSES	0x03
#define CAPTURY_STREAM_ARTAGS		0x04
#define CAPTURY_STREAM_IMAGES		0x08
#define CAPTURY_STREAM_META_DATA	0x10	// only valid when streaming poses
#define CAPTURY_STREAM_IMU_DATA		0x20
#define CAPTURY_STREAM_LATENCY_INFO	0x40

// returns 1 if successful, 0 otherwise
CAPTURY_DLL_EXPORT int Captury_startStreaming(int what);

// if you want to stream images use this function rather than Captury_startStreaming()
// returns 1 if successfull otherwise 0
CAPTURY_DLL_EXPORT int Captury_startStreamingImages(int what, int32_t cameraId);


// equivalent to Captury_startStreaming(CAPTURY_STREAM_NOTHING)
// returns 1 if successful, 0 otherwise
CAPTURY_DLL_EXPORT int Captury_stopStreaming();

// fills the pose with the current pose for the given actor
// returns the current pose. Captury_freePose() after use
CAPTURY_DLL_EXPORT CapturyPose* Captury_getCurrentPoseForActor(int actorId);
CAPTURY_DLL_EXPORT CapturyPose* Captury_getCurrentPoseAndTrackingConsistencyForActor(int actorId, int* tc);
CAPTURY_DLL_EXPORT CapturyPose* Captury_getCurrentPose(int actorId);
CAPTURY_DLL_EXPORT CapturyPose* Captury_getCurrentPoseAndTrackingConsistency(int actorId, int* tc);

// simple function for releasing memory of a pose
CAPTURY_DLL_EXPORT void Captury_freePose(CapturyPose* pose);

typedef void (*CapturyNewPoseCallback)(CapturyActor*, CapturyPose*, int trackingQuality);

// register callback that will be called when a new pose is received
// the callback will be run in a different thread than the main application
// try to be quick in the callback
// returns 1 if successful otherwise 0
CAPTURY_DLL_EXPORT int Captury_registerNewPoseCallback(CapturyNewPoseCallback callback);

typedef enum { ACTOR_SCALING, ACTOR_TRACKING, ACTOR_STOPPED, ACTOR_DELETED } CapturyActorStatus;
extern const char* CapturyActorStatusString[];
typedef void (*CapturyActorChangedCallback)(int actorId, int mode);

// register callback that will be called when a new actor is found or
// the status of an existing actor changes
// status can be one of CapturyActorStatus
// returns 1 if successful otherwise 0
CAPTURY_DLL_EXPORT int Captury_registerActorChangedCallback(CapturyActorChangedCallback callback);

typedef void (*CapturyARTagCallback)(int num, CapturyARTag*);

// register callback that will be called when an artag is detected
// pass NULL if you want to deregister the callback
// returns 1 if successful otherwise 0
CAPTURY_DLL_EXPORT int Captury_registerARTagCallback(CapturyARTagCallback callback);

// returns an array of artags followed by one where the id is -1
// Captury_freeARTags() after use
CAPTURY_DLL_EXPORT CapturyARTag* Captury_getCurrentARTags();

CAPTURY_DLL_EXPORT void Captury_freeARTags(CapturyARTag* artags);

// do NOT free the image
typedef void (*CapturyImageCallback)(const CapturyImage* img);

// register callback that will be called when a new frame was streamed from this particular camera
// pass NULL to deregister
// returns 1 if successfull otherwise 0
CAPTURY_DLL_EXPORT int Captury_registerImageStreamingCallback(CapturyImageCallback callback);

// may return NULL if no image has been received yet
// use Captury_freeImage to free after use
CAPTURY_DLL_EXPORT CapturyImage* Captury_getCurrentImage();

// requests an update of the texture for the given actor. non-blocking
// returns 1 if successful otherwise 0
CAPTURY_DLL_EXPORT int Captury_requestTexture(int actorId);

// returns the timestamp of the constraint or 0
CAPTURY_DLL_EXPORT uint64_t Captury_getMarkerTransform(int actorId, int joint, CapturyTransform* trafo);

// get the scaling status (0 - 100)
CAPTURY_DLL_EXPORT int Captury_getScalingProgress(int actorId);

// get the tracking quality (0 - 100)
CAPTURY_DLL_EXPORT int Captury_getTrackingQuality(int actorId);

// change the name of the actor
CAPTURY_DLL_EXPORT int Captury_setActorName(int actorId, const char* name);

// returns a texture image of the specified actor. free after use with Captury_freeImage().
CAPTURY_DLL_EXPORT CapturyImage* Captury_getTexture(int actorId);

// simple function for releasing memory of a pose
CAPTURY_DLL_EXPORT void Captury_freeImage(CapturyImage* image);

// synchronizes time with Captury Live
// this function should be called once before calling Captury_getTime()
// returns the current time in microseconds
CAPTURY_DLL_EXPORT uint64_t Captury_synchronizeTime();

// returns the current time as measured by Captury Live in microseconds
CAPTURY_DLL_EXPORT uint64_t Captury_getTime();

// returns the difference between the local and the remote time in microseconds
CAPTURY_DLL_EXPORT int64_t Captury_getTimeOffset();


// get the last error message
CAPTURY_DLL_EXPORT char* Captury_getLastErrorMessage();
CAPTURY_DLL_EXPORT void Captury_freeErrorMessage(char* msg);



// tries to snap an actor at the specified location
// x and z are in mm
// heading is in degrees measured from the x-axis around the y-axis (270 is facing the z-axis)
// use a value larger than 360 to indicate that heading is not known
// poll Captury_getActors to get the new actor id
// returns 1 if the request was successfully received
CAPTURY_DLL_EXPORT int Captury_snapActor(float x, float z, float heading);

// tries to snap an actor at the specified location just like Captury_snapActor()
// the additional parameters allow specifying which skeleton should be snapped (the name should match the name in the drop down)
// snapMethod should be one of CapturySnapMethod
// if quickScaling != 0 snapping uses only a single frame
// returns 1 on success, 0 otherwise
typedef enum { SNAP_BACKGROUND_LOCAL, SNAP_BACKGROUND_GLOBAL, SNAP_BODYPARTS_LOCAL, SNAP_BODYPARTS_GLOBAL, SNAP_BODYPARTS_JOINTS, SNAP_DEFAULT } CapturySnapMethod;
CAPTURY_DLL_EXPORT int Captury_snapActorEx(float x, float z, float radius, float heading, const char* skeletonName, int snapMethod, int quickScaling);

// (re-)start tracking the actor at the given location
// x and z are in mm
// heading is in degrees measured from the x-axis around the y-axis (270 is facing the z-axis)
// use a value larger than 360 to indicate that heading is not known
// returns 1 on success, 0 otherwise
CAPTURY_DLL_EXPORT int Captury_startTracking(int actorId, float x, float z, float heading);

// stops tracking the specified actor
// returns 1 on success, 0 otherwise
CAPTURY_DLL_EXPORT int Captury_stopTracking(int actorId);

// stops tracking the actor and deletes the corresponding internal data in CapturyLive
// returns 1 on success, 0 otherwise
CAPTURY_DLL_EXPORT int Captury_deleteActor(int actorId);

// rescale actor
// returns 1 on success, 0 otherwise
CAPTURY_DLL_EXPORT int Captury_rescaleActor(int actorId);

// recolor actor
// returns 1 on success, 0 otherwise
CAPTURY_DLL_EXPORT int Captury_recolorActor(int actorId);

// recolor actor
// returns 1 on success, 0 otherwise
CAPTURY_DLL_EXPORT int Captury_updateActorColors(int actorId);

#define CAPTURY_CONSTRAINT_HALF_PLANE	1
#define CAPTURY_CONSTRAINT_ROTATION	2
#define CAPTURY_CONSTRAINT_FIXED_AXIS	4
#define CAPTURY_CONSTRAINT_DISTANCE	8
#define CAPTURY_CONSTRAINT_OFFSET	16

// constrain point attached to joint to stay on one side of the half plane defined by normal, offset
// weight should nornally be 1
// returns 1 if successful, 0 otherwise
CAPTURY_DLL_EXPORT int Captury_setHalfplaneConstraint(int actorId, int jointIndex, float* originOffset, float* normal, float offset, uint64_t timestamp, float weight);

// constrain bone to point in specific direction given by x,y,z Euler angles in *rotation
// weight should nornally be 1
// returns 1 if successful, 0 otherwise
CAPTURY_DLL_EXPORT int Captury_setRotationConstraint(int actorId, int jointIndex, float* rotation, uint64_t timestamp, float weight);

// constrain axis attached to joint to point in the direction of targetAxis
// unlike the rotation constraint this constraint allows the bone to rotate around the targetAxis
// weight should nornally be 1
// returns 1 if successful, 0 otherwise
CAPTURY_DLL_EXPORT int Captury_setFixedAxisConstraint(int actorId, int jointIndex, float* axis, float* targetAxis, uint64_t timestamp, float weight);

// constrain a point attached to a joint of originActorId to maintain an offset to a point attached to a joint on targetActorId
// the offset is specified in local coordinates of the origin actor
// this constraint could for example be used to constrain the hands of a cyclist to maintain their relative position on the handle bars
// originActor and targetActor are allowed to be identical
// weight should nornally be 1
// returns 1 if successful, 0 otherwise
CAPTURY_DLL_EXPORT int Captury_setOffsetConstraint(int originActorId, int originJointIndex, float* originOffset, int targetActorId, int targetJointIndex, float* targetOffset, float* offset, uint64_t timestamp, float weight);

// constrain a point attached to a joint of originActorId to maintain a given distance to a point attached to a joint on targetActorId
// originActor and targetActor are allowed to be identical
// this constraint could for example be used to constrain the feet of a cyclist to maintain a constant distance
// weight should nornally be 1
// returns 1 if successful, 0 otherwise
CAPTURY_DLL_EXPORT int Captury_setDistanceConstraint(int originActorId, int originJointIndex, float* originOffset, int targetActorId, int targetJointIndex, float* targetOffset, float distance, uint64_t timestamp, float weight);

// constrain a point attached to a joint of originActorId to maintain a relative rotation to a point attached to a joint on targetActorId
// originActor and targetActor are allowed to be identical
// weight should nornally be 1
// returns 1 if successful, 0 otherwise
CAPTURY_DLL_EXPORT int Captury_setRelativeRotationConstraint(int originActorId, int originJointIndex, int targetActorId, int targetJointIndex, float* rotation, uint64_t timestamp, float weight);




// fills the pointers with the current day, session, shot tuple that is used in CapturyLive to identify a shot
// the strings are owned by the library - do not free or overwrite
// returns 1 if successful, 0 otherwise
CAPTURY_DLL_EXPORT int Captury_getCurrentDaySessionShot(const char** day, const char** session, const char** shot);

// sets the shot name for the next recording
// returns 1 if successful, 0 otherwise
CAPTURY_DLL_EXPORT int Captury_setShotName(const char* name);

// you have to set the shot name before starting to record - or make sure that it has been set using CapturyLive
// returns 1 if successful, 0 otherwise
CAPTURY_DLL_EXPORT int Captury_startRecording();

// returns 1 if successful, 0 otherwise
CAPTURY_DLL_EXPORT int Captury_stopRecording();

// returns 1 if successful, 0 otherwise
CAPTURY_DLL_EXPORT int Captury_getCurrentLatency(CapturyLatencyInfo* latencyInfo);


typedef void (*CapturyCustomPacketCallback)(int size, const void* data);

// send packet with any data to Captury Live
// the packet will be handled by a plugin that registered for receiving packets with this name
// the name must be at most 16 characters
// size is the size of the raw data without the name of the plugin
// returns 1 if successful, 0 otherwise
CAPTURY_DLL_EXPORT int Captury_sendCustomPacket(const char* pluginName, int size, const void* data);

// register callback that will be called when a packet with the given name is received
// the name must be at most 16 characters including a terminating 0
// the callback will be run in a different thread than the main application
// try to be quick in the callback
// returns 1 if successful, 0 otherwise
CAPTURY_DLL_EXPORT int Captury_registerCustomPacketCallback(const char* pluginName, CapturyCustomPacketCallback callback);

// convert the pose given in global coordinates into local coordinates
CAPTURY_DLL_EXPORT void Captury_convertPoseToLocal(CapturyPose* pose, int actorId);


typedef void (*CapturyBackgroundFinishedCallback)(void* userData);

CAPTURY_DLL_EXPORT int Captury_captureBackground(CapturyBackgroundFinishedCallback callback, void* userData);
CAPTURY_DLL_EXPORT int Captury_getBackgroundQuality();

CAPTURY_DLL_EXPORT const char* Captury_getStatus(); // do not free.

//
// it is safe to ignore everything below this line
//
typedef enum { capturyActors = 1, capturyActor = 2,
	       capturyCameras = 3, capturyCamera = 4,
	       capturyStream = 5, capturyStreamAck = 6, capturyPose = 7,
	       capturyDaySessionShot = 8, capturySetShot = 9, capturySetShotAck = 10,
	       capturyStartRecording = 11, capturyStartRecordingAck = 12,
	       capturyStopRecording = 13, capturyStopRecordingAck = 14,
	       capturyConstraint = 15,
	       capturyGetTime = 16, capturyTime = 17,
	       capturyCustom = 18, capturyCustomAck = 19,
	       capturyGetImage = 20, capturyImageHeader = 21, capturyImageData = 22,
	       capturyGetImageData = 23,
	       capturyActorContinued = 24,
	       capturyGetMarkerTransform = 25, capturyMarkerTransform = 26,
	       capturyGetScalingProgress = 27, capturyScalingProgress = 28,
	       capturyConstraintAck = 29,
	       capturySnapActor = 30, capturyStopTracking = 31, capturyDeleteActor = 32,
	       capturySnapActorAck = 33, capturyStopTrackingAck = 34, capturyDeleteActorAck = 35,
	       capturyActorModeChanged = 36, capturyARTag = 37,
	       capturyGetBackgroundQuality = 38, capturyBackgroundQuality = 39,
	       capturyCaptureBackground = 40, capturyCaptureBackgroundAck = 41, capturyBackgroundFinished = 42,
	       capturySetActorName = 43, capturySetActorNameAck = 44,
	       capturyStreamedImageHeader = 45, capturyStreamedImageData = 46,
	       capturyGetStreamedImageData = 47, capturyRescaleActor = 48, capturyRecolorActor = 49,
	       capturyRescaleActorAck = 50, capturyRecolorActorAck = 51,
	       capturyStartTracking = 52, capturyStartTrackingAck = 53,
	       capturyPose2 = 54,
	       capturyGetStatus = 55, capturyStatus = 56,
	       capturyUpdateActorColors = 57,
	       capturyPoseCont = 58,
	       capturyActor2 = 59, capturyActorContinued2 = 60,
	       capturyLatency = 61,
	       capturyError = 0 } CapturyPacketTypes;

// returns a string for nicer error messages
const char* Captury_getHumanReadableMessageType(CapturyPacketTypes type);

// make sure structures are laid out without padding
#pragma pack(push, 1)

// sent to server
struct CapturyRequestPacket {
	int32_t		type;		// from capturyActors, capturyCameras, capturyDaySessionShot, capturySetShot, capturyStartRecording, capturyStopRecording
	int32_t		size;		// size of full message including type and size
};

// sent to client
// as a reply to CapturyRequestPacket = capturyActors
struct CapturyActorsPacket {
	int32_t		type;	// capturyActors
	int32_t		size;	// size of full message including type and size

	int32_t		numActors;
};

// sent to client
// part of CapturyActorPacket
struct CapturyJointPacket {
	char		name[24];
	int32_t		parent;
	float		offset[3];
	float		orientation[3];
};

// sent to client
// part of CapturyActorPacket
struct CapturyJointPacket2 {
	int32_t		parent;
	float		offset[3];
	float		orientation[3];
	char		name[];		// zero terminated joint name
};

// sent to client
// as a reply to CapturyRequestPacket = capturyActors
struct CapturyActorPacket {
	int32_t		type;		// capturyActor or capturyActor2
	int32_t		size;		// size of full message including type and size

	char		name[32];
	int32_t		id;
	int32_t		numJoints;
	CapturyJointPacket	joints[];
};

// sent to client
// as a reply to CapturyRequestPacket = capturyActors
// if the CapturyActorPacket becomes too big send this one
struct CapturyActorContinuedPacket {
	int32_t		type;		// capturyActorContinued
	int32_t		size;		// size of full message including type and size

	int32_t		id;		// actor id
	int32_t		startJoint;
	CapturyJointPacket	joints[];
};

// sent to client
// as a reply to CapturyRequestPacket = capturyCameras
struct CapturyCamerasPacket {
	int32_t		type;	// capturyCameras
	int32_t		size;	// size of full message including type and size

	int32_t		numCameras;
};

// sent to client
// as a reply to CapturyRequestPacket = capturyCamera
struct CapturyCameraPacket {
	int32_t		type;	// capturyCamera
	int32_t		size;	// size of full message including type and size

	char		name[32];
	int32_t		id;
	float		position[3];
	float		orientation[3];
	float		sensorSize[2];	// in mm
	float		focalLength;	// in mm
	float		lensCenter[2];	// in mm
};

// sent to server - old version needs to stay around for old clients
struct CapturyStreamPacket0 {
	int32_t		type;		// capturyStream
	int32_t		size;		// size of full message including type and size

	int32_t		what;		// CAPTURY_STREAM_POSES or CAPTURY_STREAM_NOTHING
};

// sent to server
struct CapturyStreamPacket {
	int32_t		type;		// capturyStream
	int32_t		size;		// size of full message including type and size

	int32_t		what;		// CAPTURY_STREAM_POSES or CAPTURY_STREAM_NOTHING
	int32_t		cameraId;	// valid if what & CAPTURY_STREAM_IMAGES
};

// sent to client
// as a reply to CapturyRequestPacket = capturyDaySessionShot
struct CapturyDaySessionShotPacket {
	int32_t		type;	// capturyDaySessionShot
	int32_t		size;	// size of full message including type and size

	char		day[100];
	char		session[100];
	char		shot[100];
};

// sent to server
struct CapturySetShotPacket {
	int32_t		type;	// capturySetShot
	int32_t		size;	// size of full message including type and size

	char		shot[100];
};

// sent to client
// as a reply to CapturyStreamPacket
struct CapturyPosePacket {
	int32_t		type;	// capturyPose
	int32_t		size;	// size of full message including type and size

	int32_t		actor;
	uint64_t	timestamp;
	int32_t		numValues; // multiple of 6
	float		values[];
};

// sent to client
// as a reply to CapturyStreamPacket
struct CapturyPosePacket2 {
	int32_t		type;	// capturyPose2
	int32_t		size;	// size of full message including type and size

	int32_t		actor;
	uint64_t	timestamp;
	uint8_t		trackingQuality; // [0 .. 100]
	uint8_t		scalingProgress; // [0 .. 100]
	uint16_t	reserved;  // 0 for now
	int32_t		numValues; // multiple of 6
	float		values[];
};

struct CapturyPoseCont {
	int32_t		type;	// capturyPoseCont
	int32_t		size;	// size of full message including type and size

	int32_t		actor;
	uint64_t	timestamp;
	float		values[];
};

// sent to server
struct CapturyConstraintPacket {
	int32_t		type;	// capturyConstraint
	int32_t		size;

	int32_t		constrType;

	int32_t		originActor;
	int32_t		originJoint;
	float		originOffset[3];

	int32_t		targetActor;
	int32_t		targetJoint;
	float		targetOffset[3];

	float		targetVector[3];
	float		targetValue;
	float		targetRotation[4];

	float		weight;
};

// sent to client
// as a reply to capturyGetTime
struct CapturyTimePacket {
	int32_t		type;	// capturyTime
	int32_t		size;

	uint64_t	timestamp;
};

// sent to server
struct CapturyGetMarkerTransformPacket {
	int32_t		type;	// capturyGetMarkerTransform
	int32_t		size;

	int32_t		actor;
	int32_t		joint;
};

// sent to client
// as a reply to capturyGetMarkerTransform
struct CapturyMarkerTransformPacket {
	int32_t		type;	// capturyMarkerTransform
	int32_t		size;

	uint64_t	timestamp;

	int32_t		actor;
	int32_t		joint;

	float		rotation[3]; // XYZ Euler angles
	float		translation[3];
};

// sent to server
struct CapturyGetScalingProgressPacket {
	int32_t		type;	// capturyGetScalingProgress
	int32_t		size;

	int32_t		actor;
};

// sent to client
// as a reply to capturyGetScalingProgress
struct CapturyScalingProgressPacket {
	int32_t		type;		// capturyScalingProgress
	int32_t		size;

	int32_t		actor;
	int8_t		progress;	// value from 0 to 100
};

// sent in both directions
// a CapturyRequestPacket = capturyCustomAck is always sent in reply
struct CapturyCustomPacket {
	int32_t		type;	// capturyCustom
	int32_t		size;

	char		name[16];
	char		data[];
};

// sent to server
struct CapturyGetImagePacket {
	int32_t		type;	// capturyGetImage
	int32_t		size;

	int32_t		actor;
};

struct CapturyGetImageDataPacket {
	int32_t		type;	// capturyGetImageData
	int32_t		size;

	uint16_t	port;
	int32_t		actor;
};

// sent to client
// as a reply to capturyGetImage or for streamed cameras
//
struct CapturyImageHeaderPacket {
	int32_t		type;		// capturyImageHeader, capturyStreamedImageHeader
	int32_t		size;

	int32_t		actor;		// for capturyStreamedImageHeader this is the camera id
	uint64_t	timestamp;
	uint32_t	fourcc;		// image compression format
	int32_t		width;
	int32_t		height;
	int32_t		dataPacketSize;	// size of data packets
	int32_t		dataSize;
};

struct CapturyImageDataPacket {
	int32_t		type;	// capturyImageData, capturyStreamedImageData
	int32_t		size;

	int32_t		actor;	// for capturyStreamedImageData this is the camera id
	int32_t		offset;	// offset in bytes into the following data (0 for first packet)
	unsigned char	data[];	// width*height*3 bytes
};

struct CapturySnapActorPacket {
	int32_t		type; // capturySnapActor
	int32_t		size;
	float		x;
	float		z;
	float		heading;
};

struct CapturySnapActorPacket2 {
	int32_t		type; // capturySnapActor
	int32_t		size;
	float		x;
	float		z;
	float		radius;
	float		heading;
	uint8_t		snapMethod;
	uint8_t		quickScaling;
	char		skeletonName[32];
};

struct CapturyStartTrackingPacket {
	int32_t		type; // capturyStartTracking
	int32_t		size;

	int32_t		actor;
	float		x;
	float		z;
	float		heading;
};

struct CapturyStopTrackingPacket {
	int32_t		type; // capturyStopTracking or capturyDeleteActor or capturyRescaleActor or capturyRecolorActor
	int32_t		size;
	int32_t		actor;
};

struct CapturyActorModeChangedPacket {
	int32_t		type; // capturyActorModeChanged
	int32_t		size;
	int32_t		actor;
	int32_t		mode;
};

struct CapturyARTagPacket {
	int32_t		type; // capturyARTag
	int32_t		size;
	int32_t		numTags;

	CapturyARTag	tags[1];
};

struct CapturyBackgroundQualityPacket {
	int32_t		type; // capturyBackgroundQuality
	int32_t		size;
	int32_t		quality;
};

struct CapturySetActorNamePacket {
	int32_t		type; // capturySetActorName
	int32_t		size;
	int32_t		actor;
	char		name[32];
};

struct CapturyStatusPacket {
	int32_t		type; // capturyStatus
	int32_t		size;
	char		message[1]; // 0-terminated
};

// sent to client
// as a reply to CapturyStreamPacket
struct CapturyIMUData {
	int32_t		type;	// capturyIMU
	int32_t		size;	// size of full message including type and size

	uint8_t		numIMUs;
	float		eulerAngles[]; // 3x numIMUs floats
};

// sent to client
// as a reply to CapturyStreamPacket
struct CapturyLatencyPacket {
	int32_t		type;	// capturyLatency
	int32_t		size;	// size of full message including type and size

	uint64_t	firstImagePacket;
	uint64_t	optimizationStart;
	uint64_t	optimizationEnd;
	uint64_t	sendPacketTime; // right before packet is sent

	uint64_t	poseTimestamp;	// timestamp of corresponding pose
};

#pragma pack(pop)

#ifndef FOURCC
#define FOURCC(a,b,c,d)		(((d)<<24)|((c)<<16)|((b)<<8)|(a))
#endif

#define FOURCC_RGB		FOURCC('2','4',' ',' ') // uncompressed RGB

#ifdef __cplusplus
} // extern "C"
#endif
