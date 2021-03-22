#pragma once

#include <stdint.h>

typedef uint32_t uint;

#pragma pack(push, 1)

struct CapturyJoint {
	char		name[64];
	int		parent;		// index of parent joint or -1 for root node
	float		offset[3];	// offset to parent joint
	float		orientation[3];	// XYZ quaternion - w needs to be reconstructed (w >= 0)
};

struct CapturyBlob {
	int		parent;		// index of parent joint or -1 for root node
	float		offset[3];	// offset to parent joint
	float		size;		// radius
	float		color[3];	// RGB color [0..1]
};

struct CapturyActor {
	char		name[32];
	int		id;
	int		numJoints;
	CapturyJoint*	joints;
	int		numBlobs;
	CapturyBlob*	blobs;
	int		padding;	// pad to 8 byte alignment
};

struct CapturyTransform {
	float		translation[3];	// translation
	float		rotation[3];	// XYZ Euler angles
};

struct CapturyPose {
	int			actor;
	uint64_t		timestamp;	// in microseconds - since the start of Captury Live
	int			numTransforms;
	CapturyTransform*	transforms;	// one CapturyTransform per joint in global (world space) coordinates
						// the transforms are in the same order as the joints
						// in the corresponding CapturyActor.joints array
};

struct CapturyIMUPose {
	uint			imu;
	uint64_t		timestamp;	// in microseconds - since the start of Captury Live
	float			orientation[3];
	float			acceleration[3];// linear acceleration
	float			position[3];
};

struct CapturyConstraint {
	int		actor;
	char		jointName[24];
	CapturyTransform transform;
	float		weight;
	char		constrainTranslation;	// if 0 ignore the translational part of the transform
	char		constrainRotation;	// if 0 ignore the rotational part of the transform
};

struct CapturyCamera {
	char		name[32];
	int		id;
	float		position[3];
	float		orientation[3];
	float		sensorSize[2];	// in mm
	float		focalLength;	// in mm
	float		lensCenter[2];	// in mm
	char		distortionModel[16]; // name of distortion model or "none"
	float		distortion[30];

	// the following can be computed from the above values and are provided for convenience only
	// the matrices are stored column wise:
	// 0  3  6  9
	// 1  4  7 10
	// 2  5  8 11
	float		extrinsic[12];
	float		intrinsic[9];
};

struct CapturyImage {
	int		width;
	int		height;
	int		camera;		// camera index
	uint64_t	timestamp;	// in microseconds
	uint8_t*	data;		// packed image data: stride = width*3 bytes
	uint8_t*	gpuData;
};

struct CapturyDepthImage {
	int		width;
	int		height;
	int		camera;
	uint64_t	timestamp;	// in microseconds
	uint16_t*	data;		// packed image data: stride = width*2 bytes
};

struct CapturyARTag {
	int32_t			id;

	CapturyTransform	transform;
};

struct CapturyCornerDetection {
	int32_t		camera;
	float		position[2];
	float		boardCoordinates[3];
};

struct CapturyLatencyInfo {
	uint64_t	firstImagePacketTime;
	uint64_t	optimizationStartTime;
	uint64_t	optimizationEndTime;
	uint64_t	poseSentTime;
	uint64_t	poseReceivedTime;

	uint64_t	timestampOfCorrespondingPose;
};

#pragma pack(pop)
