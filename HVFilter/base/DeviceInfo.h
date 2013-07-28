#pragma once

#define OSR_COMM_CONTROL_DEVICE_NAME_PREFIX L"\\Device\\OSR"
#define OSR_COMM_CONTROL_DEVICE_NAME_SUFFIX L"Control"
#define OSR_COMM_DATA_DEVICE_NAME_PREFIX L"\\Device\\OSR"
#define OSR_COMM_DATA_DEVICE_NAME_SUFFIX L"Data"
#define OSR_COMM_DOSDEVICE_PATH L"\\DosDevices\\OSR"


typedef struct _OSR_COMM_CONTROL_DEVICE_EXTENSION {

  //
  // Data structure magic #
  //
  ULONG MagicNumber;

  //
  // Registry Path
  //
  UNICODE_STRING RegistryPath;

  //
  // Driver Name
  //
  UNICODE_STRING DriverName;

  //
  // Symbolic Link Name
  //
  UNICODE_STRING SymbolicLinkName;

  //
  // Control Thread Service queue
  //
  LIST_ENTRY ServiceQueue;

  //
  // Control Thread Service Queue Lock
  //
  FAST_MUTEX ServiceQueueLock;

  //
  // Control Request Queue - awaiting dispatch to control threads
  //
  LIST_ENTRY RequestQueue;

  //
  // Control Request Queue Lock
  //
  FAST_MUTEX RequestQueueLock;

} OSR_COMM_CONTROL_DEVICE_EXTENSION, *POSR_COMM_CONTROL_DEVICE_EXTENSION;

#define OSR_COMM_CONTROL_EXTENSION_MAGIC_NUMBER 0x1d88f403

typedef struct _OSR_COMM_DATA_DEVICE_EXTENSION {

  //
  // Data structure magic #
  //
  ULONG MagicNumber;

  //
  // This is used to indicate the state of the device
  //
  ULONG DeviceState;

  //
  // Symbolic Link Name
  //
  UNICODE_STRING SymbolicLinkName;

  //
  // Read Request Queue
  //
  LIST_ENTRY ReadRequestQueue;

  //
  // Read Request Queue Lock
  //
  FAST_MUTEX ReadRequestQueueLock;

  //
  // Write Request Queue
  //
  LIST_ENTRY WriteRequestQueue;

  //
  // Write Request Queue Lock
  //
  FAST_MUTEX WriteRequestQueueLock;

} OSR_COMM_DATA_DEVICE_EXTENSION, *POSR_COMM_DATA_DEVICE_EXTENSION;

#define OSR_COMM_DATA_DEVICE_EXTENSION_MAGIC_NUMBER 0x34df009b

//
// Active = allows queueing requests
// Inactive = disallows queuing requests
//
#define OSR_COMM_DATA_DEVICE_ACTIVE   0x10
#define OSR_COMM_DATA_DEVICE_INACTIVE 0x20