#pragma once

//#include <ndis.h>
//#include <ntifs.h>

//
// OsrCommCreate
//
//  This is the create entry point
//
// Inputs:
//  DeviceObject - this is the device object on which we are operating
//  Irp - this is the create IRP
//
// Outputs:
//  None.
//
// Returns:
//  SUCCESS - the operation was successful.
//
// Notes:
//  None.
//
NTSTATUS OsrCommCreate(PDEVICE_OBJECT DeviceObject, PIRP Irp);

//
// 
//OsrCommCleanup
//  This is the cleanup entry point
//
// Inputs:
//  DeviceObject - this is the device object on which we are operating
//  Irp - this is the cleanup IRP
//
// Outputs:
//  None.
//
// Returns:
//  SUCCESS - the operation was successful.
//
// Notes:
//  None.
//
NTSTATUS OsrCommCleanup(PDEVICE_OBJECT DeviceObject, PIRP Irp);

//
// Close
//
//  This is the close entry point
//
// Inputs:
//  DeviceObject - this is the device object on which we are operating
//  Irp - this is the close IRP
//
// Outputs:
//  None.
//
// Returns:
//  SUCCESS - the operation was successful.
//
// Notes:
//  None.
//
NTSTATUS OsrCommClose(PDEVICE_OBJECT DeviceObject, PIRP Irp);


//
// ProcessResponse
//
//  This routine is used to process a response
//
// Inputs:
//  Irp - this is the IRP containing a (validated) response
//
// Outputs:
//  None.
//
// Returns:
//  STATUS_SUCCESS - the operation completed successfully
//
// Notes:
//  This is a helper function for the device control logic.  It does NOT
//  complete the control request - that is the job of the caller!  It DOES
//  complete the data request (if it finds a matching entry).
//
NTSTATUS ProcessResponse(PIRP Irp);

//
// ProcessControlRequest
//
//  This routine is used to either satisfy the control request or enqueue it
//
// Inputs:
//  Irp - this is the IRP that we are processing
//  ControlRequest - this is the control request (from the IRP, actually)
//
// Outputs:
//  None.
//
// Returns:
//  SUCCESS - there's data going back up to the application
//  PENDING - the IRP will block and wait 'til it is time...
//
//
NTSTATUS ProcessControlRequest(PIRP Irp);

//
// OsrCommReadWrite
//
//  This is the read/write entry point
//
// Inputs:
//  DeviceObject - this is the device object on which we are operating
//  Irp - this is the read IRP
//
// Outputs:
//  None.
//
// Returns:
//  SUCCESS - the operation was successful.
//
// Notes:
//  The operation is common for both read/write, except that it uses the "correct" queue
//  for each operation.
//
NTSTATUS OsrCommReadWrite(PDEVICE_OBJECT DeviceObject, PIRP Irp);

//
// OsrCommDeviceControl
//
//  This is the device control entry point
//
// Inputs:
//  DeviceObject - this is the device object on which we are operating
//  Irp - this is the device control IRP
//
// Outputs:
//  None.
//
// Returns:
//  SUCCESS - the operation was successful.
//
// Notes:
//  None.
//
NTSTATUS OsrCommDeviceControl(PDEVICE_OBJECT DeviceObject, PIRP Irp);