/** @file
 **/
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
//#include <Library/UefiRuntimeLib.h>
#include <Protocol/GraphicsOutput.h>
#include <Protocol/SimplePointer.h>
#include <Protocol/AbsolutePointer.h>
#include <Protocol/SimpleTextInEx.h>

//#pragma warning(disable: 4702)
/** example  
 *
 */

EFI_STATUS
TestInput()
{
    EFI_STATUS  Status;
    EFI_EVENT   myEvents[2];
    UINTN       index = 0;

    Status = gBS->CreateEvent(EVT_TIMER,
                              TPL_CALLBACK,
                              (EFI_EVENT_NOTIFY)NULL,
                              (void *)NULL,
                              &myEvents[0]);

    Status = gBS->SetTimer(myEvents[0], TimerRelative, 10 * 1000 * 1000);

    myEvents[1] = gST->ConIn->WaitForKey;


    while(1)
    {
        Status = gBS->WaitForEvent(2, myEvents, &index);
        Status = gBS->SetTimer(myEvents[0], TimerCancel, 10 * 1000 * 1000);

        if(index == 0)
        {
            gST->ConOut->OutputString(gST->ConOut, L"Timer time out!\n\r");
        }else if(index == 1)
        {
            EFI_INPUT_KEY Key;
            Status = gST->ConIn->ReadKeyStroke(gST->ConIn, &Key);
            gST->ConOut->OutputString(gST->ConOut, &Key.UnicodeChar);
            break;
        }

        Status = gBS->SetTimer(myEvents[0], TimerRelative, 10 * 1000 * 1000);

    }
    return Status;
}

/** example  1
 *
 */

VOID
WaitKey()
{
    EFI_STATUS Status = 0;
    UINTN index = 0;
    EFI_INPUT_KEY Key;

    Status = gBS->WaitForEvent(1, &gST->ConIn->WaitForKey, &index);
    Status = gST->ConIn->ReadKeyStroke(gST->ConIn, &Key);
}

/** example  2
 *
 */

UINTN TimeOut()
{
    return 0;
}

EFI_STATUS
TestTimer()
{
    EFI_STATUS  Status;
    EFI_EVENT   myEvent;
    UINTN       index;
    UINTN       repeats = 0;

    Status = gBS->CreateEvent(EVT_TIMER,
                              TPL_CALLBACK,
                              (EFI_EVENT_NOTIFY)NULL,
                              (VOID*)NULL,
                              &myEvent);
    if(EFI_ERROR(Status))
    {
        Print(L"FAIL EVT_TIMER | EVT_RUNTIME %r!\n\r", Status);
        return Status;
    }

    Status = gBS->SetTimer(myEvent, TimerPeriodic, 10 * 1000 * 1000);
    while(1)
    {
        Status = gBS->WaitForEvent(1, &myEvent, &index);
        TimeOut();
        Print(L"Time out!\n\r");
        if (repeats++ == 10)
        {
            break;
        }
    }

    Status = gBS->CloseEvent(myEvent);
    return Status;
}

/** example  3
 *
 */

VOID
myEventNotify(
        IN EFI_EVENT        Event,
        IN VOID             *Context
        )
{
    static UINTN Times = 1;
    Print(L"myEventNotify Wait %d times\n\r", Times);
    if(Times++ > 5)
        gBS->SignalEvent(Event);
}

EFI_STATUS
TestNotify()
{
    EFI_STATUS  Status;
    EFI_EVENT   myEvent;
    UINTN       index;

    Status = gBS->CreateEvent(EVT_NOTIFY_WAIT,
                              TPL_NOTIFY,
                              (EFI_EVENT_NOTIFY)myEventNotify,
                              (VOID *)NULL,
                              &myEvent);

    Status = gBS->WaitForEvent(1, &myEvent, &index);

    return Status;
}

/** example  4
 *
 */

VOID
myEventNotify30(
        IN EFI_EVENT    Event,
        IN VOID         *Context)
{
    static UINTN times = 1;
    Print(L"%s myEventNotify signal %d times!\n\r", Context, times);
    times++;
}

EFI_STATUS
TestEventSignal()
{
    EFI_STATUS  Status;
    EFI_EVENT   myEvent;

    Print(L"Test EVT_TIMER | EVT_NOTIFY_SIGNAL\n\r");
    Status = gBS->CreateEvent(EVT_TIMER | EVT_NOTIFY_SIGNAL,
                              TPL_CALLBACK,
                              (EFI_EVENT_NOTIFY)myEventNotify30,
                              (VOID *)L"Hello timeout",
                              &myEvent);
    Status = gBS->SetTimer(myEvent, TimerPeriodic, 100 * 1000 * 1000);
    WaitKey();

    Status = gBS->CloseEvent(myEvent);

    return Status;
}


/** example  4
 *
 */
EFI_STATUS
TestMouse()
{
    EFI_STATUS                      Status;
    EFI_SIMPLE_POINTER_PROTOCOL     *Mouse = 0;
    EFI_SIMPLE_POINTER_STATE        State;
    EFI_EVENT                       Events[2]; //= {0, gST->ConIn->WaitForKey};


    Print(L"test1\n\r");
    gST->ConOut->EnableCursor(gST->ConOut, TRUE);

    Print(L"test2\n\r");

    Status = gBS->LocateProtocol(&gEfiSimplePointerProtocolGuid,
                                 NULL,
                                 (VOID **)&Mouse);
    if(EFI_ERROR(Status))
    {
        Print(L"The error code is %x\n\r", Status);
        return Status;
    }

    Print(L"test3\n\r");
    Status = Mouse->Reset(Mouse,TRUE);

    Print(L"test4\n\r");

    Events[1] = gST->ConIn->WaitForKey;
    Events[0] = Mouse->WaitForInput;

    while(1)
    {
        EFI_INPUT_KEY       Key;
        UINTN               Index;

        Print(L"Start to wait...\n\r");

        Status = gBS->WaitForEvent(2, Events, &Index);

        if(Index == 0)
        {
            Status = Mouse->GetState(Mouse, &State);
            Print(L"X: %d Y: %d Z: %d L: %d R: %d\n\r",
                  State.RelativeMovementX,
                  State.RelativeMovementY,
                  State.RelativeMovementZ,
                  State.LeftButton,
                  State.RightButton);
        }else if(Index ==  1)
        {
            Status = gST->ConIn->ReadKeyStroke(gST->ConIn, &Key);
            gST->ConOut->OutputString(gST->ConOut, &Key.UnicodeChar);

            if(Key.UnicodeChar == 'q')
                break;
        }
    }
    return Status;
}

/** example  5
 *
 */
VOID
MyTimerNotify(
        IN EFI_EVENT    Event,
        IN VOID         *Context)
{
    static UINTN times = 1;
    Print(L"Notify: %ds\n", times);
    times++;
}

EFI_STATUS
TimerEventTest()
{
    EFI_STATUS  Status;
    EFI_EVENT   MyEvent;

    Status = gBS->CreateEvent(
                EVT_TIMER | EVT_NOTIFY_SIGNAL,
                TPL_CALLBACK,
                (EFI_EVENT_NOTIFY)MyTimerNotify,
                NULL,
                &MyEvent
                );

    if(EFI_ERROR(Status))
        return Status;

    Status = gBS->SetTimer(
                MyEvent,
                TimerPeriodic,
                10 * 1000 * 1000
                );
    WaitKey();
    Status = gBS->CloseEvent(MyEvent);

    return Status;

}

EFI_STATUS
UefiMain
(
IN EFI_HANDLE               ImageHandle,
IN EFI_SYSTEM_TABLE         *SystemTable
)
{
    EFI_STATUS Status;
    //Status = TestInput();
    //Status = TestTimer();
    //Status = TestNotify();
    //Status  = TestEventSignal();
    //Status = TestMouse();
    Status = TimerEventTest();
    return Status;
}

//#pragma warning(default: 4702)
