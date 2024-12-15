#pragma once

#include "config/Export.h"

#include <cstdarg>
#include <cstdio>
#include <typeinfo>
#include <sstream>

namespace dagbase
{
    enum Errod
    {
        //! Success.
        NoError,
        //! Null pointer when valid pointer was expected.
        NullArgument,
        //! A generic error occurred during processing of a state.
        IllegalState,
        //! The peer in an event doesn't correspond to peer in a Connection.
        MismatchedPeer,
        //! Tried to connect to an already connected Connection.
        AlreadyConnected,
        // Tried to re-connect to a closed Connection.
        AlreadyClosed,
        //! enet_create_host() for a server failed.
        FailedToCreateServer,
        //! enet_create_host() for a client failed.
        FailedToCreateClient,
        //! enet_host_connect() failed.
        FailedToCreatePeer,
        //! enet_host_connect() failed.
        FailedToConnect,
        //! Did not get an ack for a enet_host_connect().
        ConnectionRefused,
        // Name resolution failed.
        FailedToResolveName,
        //! index into array or other container.
        IndexOutOfRange,
        //! Data was received on an unexpected channel.
        UnexpectedChannel,
        //! An unexpected command was received.
        UnexpectedCommand,
        //! A connection happened when the current state does not expect it.
        UnexpectedConnection,
        //! A disconnection happened when the current state does not expect it.
        UnexpectedDisconnection,
        //! Typically not found or read permission.
        FailedToOpenConfigFile,
        //! Unexpected value of a parameter.
        BadConfigParameter,
        //! A module with an unknown type was specified.
        InvalidModuleType,
        //! The arguments were well-formed but invalid.
        ComponentInitError,
        //! An object to implement Control could not be created.
        FailedToCreateControlDevice,
        //! An object to implement Dynamics could not be created.
        FailedToCreateDynamics,
        //! Bad combination of stages in KitchenSinkApp.
        InvalidStageCombination,
        //! An error in the data accompanying a command.
        BadCommandData,
        //! Invalid data in StartupData packet.
        BadStartupData,
        //! CmdStartRendering had invalid parameters.
        BadStartRendering,
        //! CmdReady had invalid parameters.
        BadReady,
        //! CmdFrameData had invalid parameters.
        BadFrameData,
        //! ControlInputs had invalid parameters.
        BadControlInputs,
        //! ControlFeedback had invalid parameters.
        BadControlFeedback,
        //! DynamicsData had invalid parameters.
        BadDynamics,
        //! SwapInterval is not implemented.
        SwapIntervalNotImplemented,
        //! The call to SwapInterval failed unexpectedly.
        SwapIntervalFailed,
        //! The functions related to framelock are not implemented.
        FramelockNotImplemented,
        //! An invalid number of groups or barriers.
        NotEnoughGroupsOrBarriers,
        //! The query for the maximum group failed.
        QueryMaxGroupsFailed,
        //! The call to join a group or bind to a barrier failed.
        JoiningOrBindingFailed,
        //! The call to reset the frame counter failed.
        ResetFrameCountFailed,
        //! The loading of the camera configuration file failed.
        FailedToLoadCameraConfig,
        //! The loading of a scene failed.
        FailedToLoadScene,
        //! The creation of channels failed.
        FailedToCreateChannels,
        //! A camera could not be found.
        FailedToFindCamera,
        //! Generic error for an operation that should not happen.
        InvalidOperation,
        //! An out-of-range channel index
        InvalidChannelIndex,
        //! An out-of-range scene index
        InvalidSceneIndex,
        // Generic invalid argument.
        InvalidArgument,
        //! Failure using a realtime synchronisation primitive in xenomai.
        RealtimeSyncError,
        //! Failure using rtnet.
        RealtimeNetworkError,
        //! Failure relating to a realtime task in xenomai.
        RealtimeTaskError,
        //! Creation of a socket failed.
        FailedToCreateSocket,
        //! Binding an address failed.
        FailedToBindLocalAddress,
        //! Setting broadcast mode failed
        FailedToSetBroadcastMode,
        //! An address was already in use
        FailedToReuseAddress,
        //! A file unexpectedly exists.
        FileAlreadyExists,
        //! A file could not be found.
        FileNotFound,
        //! The end of a file was reached unexpectedly
        EndOfFile,
        //! An invalid model handle was used.
        InvalidModel,
        //! A View3D could not be created, typically because a sub-component failed.
        FailedToCreateView,
        //! A part could not be found even when the Renderer is available.
        InvalidView,
        //! A factory method failed because an unknown class name was given
        UnknownClass,
        //! Mismatch between registry and handle passed to operation.
        ClassMismatch,
        //! Attempt to register a class more than once
        ClassAlreadyRegistered,
        //! A component of a path does not exist. e.g. /foo/bar/baz.txt where bar is not a directory.
        PathComponentNotFound,
        //! A road lookup failed.
        RoadNotFound,
        //! An invalid enumeration was used.
        InvalidEnum,
        //! Generic object lookup failure.
        ObjectNotFound,
        //! Typically a text file has an unexpected keyword.
        UnexpectedKeyword,
        //! A stream was corrupted.
        BadStreamError,
        //! Typically some bad numbers.
        UnexpectedOutput,
        //! An unexpected number of cameras was found in the configuration file
        CameraCountMismatch,
        //! Typically a text configuration file.
        SyntaxError,
        //! We cannot process because there is insufficient data.
        NotEnoughMeasurements,
        //! There was insufficient time to complete processing.
        NotEnoughTime,
        //! There was some kind of error reading an existing road network from a file.
        FailedToBuildRoadNetwork,
        //! We failed to create a log file typically due to incorrect permissions or exclusive access by another application.
        FailedToCreateLogFile,
        //! We failed to set the position of an object typically using the LRN.
        FailedToSetPosition,
        //! Dynamics failed to run for some unspecified reason.
        FailedToRunDynamics,
        //! We were unable to load the parts of a View3D.
        FailedToLoadViewParts,
        //! We received data from e.g. the network when we are not connected or in an error state.
        UnexpectedData,
        //! An unexpected version number was encountered in the DataDumpFile.
        UnexpectedDumpFileVersion,
        //! An unexpected version number was encountered in the network protocol.
        UnexpectedProtocolVersion,
        //! An intersection calculation failed.
        FailedToGetIntersection,
        //! A distance to plane calculation yielded an unexpected value e.g. non-zero when it should be zero.
        UnexpectedDistanceFromPlane,
        //! A method is currently not implemented
        MethodNotImplemented,
        //! Marker for the last error code.
        LastError
    };

    DAGBASE_API const char * ErrorCodeToName( int code );

    #define CHECK_STAGE_OK( stage )                             \
    {                                                           \
        int stageErrod = NoError;                               \
                                                                \
        if ( !(ok) || ( stageErrod = (stage) ) != NoError )     \
        {                                                       \
            return stageErrod;                                  \
        }                                                       \
    }

    #define CHECK_STAGE_SOURCE_OK( stage, filename, lineno )    \
    {                                                           \
        int stageErrod = NoError;                               \
                                                                \
        if ( !(ok) || ( stageErrod = (stage) ) != NoError )     \
        {                                                       \
            return stageErrod;                                  \
        }                                                       \
    }

    #define BEGIN_ERROR_TABLE(tableName) sim::ErrorInfo tableName[] = {

    #define ERROR_INFO(name,desc) { " SIM_TOSTRING(name) ", " SIM_TOSTRING(desc) " },

    #define END_ERROR_TABLE() }

    struct DAGBASE_API ErrorDescriptor
    {
        int code;
        const char* name;
        const char* description;
    };
    
    struct ErrorInfo
	{
		const char * name;
		const char * desc;
	};

    //! Raise an exception with the given description of the form \verbatim <typename of exception>:source file <nn>\tline number <nn>\tcode: <description>\tmessage: <formatted message> \endverbatim
    //! @note varargs is not type-safe, so enable format-string warnings with -Wformat etc, included in -Wall.
    //! Limit of 256 characters on the buffer for the varargs string.
    template<typename Exception>
    void raiseError( int code, const char * sourceFile, int lineNumber, const char * format, ... )
    {
        std::ostringstream errorStr;
        char buf[256];

        va_list args;

        va_start( args, format );
            vsnprintf( buf, 255, format, args );
        va_end(args);
        errorStr << typeid(Exception).name() << ":source file: " << sourceFile << "\tline: " << lineNumber << "\tcode: " << ErrorCodeToName(code) << "\tmessage: " << buf;
            throw Exception( errorStr.str() );
    }
}
