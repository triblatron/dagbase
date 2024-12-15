#include "config/config.h"

#include "util/errod.h"

namespace dagbase
{

    static const char* Errod_descriptions[] = {
        "NoError",
        "NullArgument",
        "IllegalState",
        "MismatchedPeer",
        "AlreadyConnected",
        "AlreadyClosed",
        "FailedToCreateServer",
        "FailedToCreateClient",
        "FailedToCreatePeer",
        "FailedToConnect",
        "ConnectionRefused",
        "FailedToResolveName",
        "IndexOutOfRange",
        "UnexpectedChannel",
        "UnexpectedCommand",
        "UnexpectedConnection",
        "UnexpectedDisconnection",
        "FailedToOpenConfigFile",
        "BadConfigParameter",
        "InvalidModuleType",
        "ComponentInitError",
        "FailedToCreateControlDevice",
        "FailedToCreateDynamics",
        "InvalidStageCombination",
        "BadCommandData",
        "BadStartupData",
        "BadStartRendering",
        "BadReady",
        "BadFrameData",
        "BadControlInputs",
        "BadControlFeedback",
        "BadDynamics",
        "SwapIntervalNotImplemented",
        "SwapIntervalFailed",
        "FramelockNotImplemented",
        "NotEnoughGroupsOrBarriers",
        "QueryMaxGroupsFailed",
        "JoiningOrBindingFailed",
        "ResetFrameCountFailed",
        "FailedToLoadCameraConfig",
        "FailedToLoadScene",
        "FailedToCreateChannels",
        "FailedToFindCamera",
        "InvalidOperation",
        "InvalidChannelIndex",
        "InvalidSceneIndex",
        "InvalidArgument",
        "RealtimeSyncError ( non-realtime thread? )",
        "RealtimeNetworkError( sendto/recvfrom failed? )",
        "RealtimeTaskError( create, delete, set_periodic failed? )",
        "FailedToCreateSocket",
        "FailedToBindLocalAddress",
        "FailedToSetBroadcastMode",
        "FailedToReuseAddress",
        "FileAlreadyExists",
        "FileNotFound",
        "EndOfFile",
        "InvalidModel",
        "FailedToCreateView",
        "InvalidView",
        "UnknownClass",
        "ClassMismatch",
        "ClassAlreadyRegistered",
        "PathComponentNotFound",
        "RoadNotFound",
        "InvalidEnum",
        "ObjectNotFound",
        "UnexpectedKeyword",
        "BadStreamError",
        "UnexpectedOutput",
        "CameraCountMismatch",
        "SyntaxError",
        "NotEnoughMeasurements",
        "NotEnoughTime",
        "FailedToBuildRoadNetwork",
        "FailedToCreateLogFile",
        "FailedToSetPosition",
        "FailedToRunDynamics",
        "FailedToLoadViewParts",
        "UnexpectedData",
        "UnexpectedDumpFileVersion",
        "UnexpectedProtocolVersion",
        "FailedToGetIntersection",
        "UnexpectedDistanceFromPlane",
        "MethodNotImplemented",
        "LastError"
    };

    const char* ErrorCodeToName(int code)
    {
        if (code < 0 || code > LastError)
        {
            return "Unknown error";
        }

        return Errod_descriptions[code];
    }
}
