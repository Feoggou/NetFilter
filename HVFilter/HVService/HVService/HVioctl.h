#pragma once

#include <Windows.h>

typedef struct _OSR_COMM_CONTROL_REQUEST {
  //
  // The request ID is used to match up the response to the original request
  //
  ULONG RequestID;

  //
  // The request type indicates the operation to be performed
  //
  ULONG RequestType;

  //
  // The data buffer allows the application to receive arbitrary data
  // Note that this is done OUT OF BOUNDS from the IOCTL.  Thus, the driver
  // is responsible for managing this.
  //
  PVOID RequestBuffer;

  //
  // This specifies the size of the request buffer
  //
  ULONG RequestBufferLength;


} OSR_COMM_CONTROL_REQUEST, *POSR_COMM_CONTROL_REQUEST;

typedef struct _OSR_COMM_CONTROL_RESPONSE {
  //
  // The request ID is used to match up this response to the original request
  //
  ULONG RequestID;

  //
  // The response type indicates the type of response information
  //
  ULONG ResponseType;

  //
  // The data buffer allows the application to return arbitrary data
  // Note that this is done OUT OF BOUNDS from the IOCTL.  Thus, the driver
  // should not trust this data field to be correct.
  //
  PVOID ResponseBuffer;

  ULONG ResponseBufferLength;

} OSR_COMM_CONTROL_RESPONSE, *POSR_COMM_CONTROL_RESPONSE;