#include "precomp.hpp"

#include "Lock.tmh"


/* 
Closes a gate. If there are pending Enters into the gate,
the close will complete asynchronously. 

\note Once Close is called, it CANNOT be called again
      until its open again

\return 
     TRUE if close is complete.  
     FALSE if completion will be signalled asynchronously 
         (from a Leave call).
*/
BOOLEAN CWFCExclusiveAsyncGate::CloseAsync(
    _In_opt_ IOperationCompletionCallback * completionCallback)
{
    WFC_COUNTED_GATE_REFERENCE_OBJECT NewValue;

    //
    // set the completion event pointer first, so the Leave() wont miss it
    //
    InterlockedExchangePointer ( (PVOID volatile *)(&m_CloseCompletionCallback), 
                                (PVOID)(completionCallback));

    //
    // ReferenceObject.Closed = TRUE;      // Mark Closed
    // ReferenceObject.ReferenceCount--;   // Decrement ReferenceCount
    // The former is achieved by adding 1 to the reference object value
    // and the latter by subtracting 2 from it.  We decrement by 1 to
    // achieve the combined effect!
    //
    NewValue.Value = InterlockedDecrement (&ReferenceObject.Value);

    //
    // Closed should never get marked more than once.  A successive
    // attempt to do so would flip this bit resulting in an assert.
    //
    NT_ASSERT(NewValue.Closed);

    //
    // If ReferenceCount falls to 0 and Closed has been set, then
    // indicate to the caller that the object can now be Closed
    //
    if (NewValue.Value == 1) 
    {
        //
        // reset the completion event pointer
        //
        InterlockedExchangePointer ((PVOID volatile *)&m_CloseCompletionCallback, NULL);
        return TRUE;
    }

    // Not closed yet, close will complete asynchronously
    return FALSE;
}

/*
 Allows specified number of "counters" to leave. If 
 a Close has been pending and all Enters have exited, the
 Close can now be completed

 \return
      TRUE to signal completion of a previous close request.
      FALSE otherwise.
*/
BOOLEAN CWFCExclusiveAsyncGate::Leave()
{
    NDIS_STATUS ndisStatus;
    WFC_COUNTED_GATE_REFERENCE_OBJECT NewValue;
            
    IOperationCompletionCallback *closeCompletionCallback;
    EventQueue *eventQueue;
      
    //
    // ReferenceObject.ReferenceCount -= Count;
    // This is achieved by subtracting (2 * Count).
    //
    NewValue.Value = InterlockedExchangeAdd(&ReferenceObject.Value, 
                                            -2);

    //
    // InterlockedExchangeAdd returns the original value!
    //
    NewValue.Value -= 2;
                
    //
    // Did we take away more references than we actually had?
    // That would have caused the most significant bit to get set!
    //
    NT_ASSERT(!NewValue.Overflow);

    //
    // ReferenceCount should never fall to 0 before Closed gets set.
    //
    NT_ASSERT(NewValue.Value != 0);

    //
    // If ReferenceCount falls to 0 and Closed has been set, then
    // indicate to the caller that the object can now be Closed.
    //
    if (NewValue.Value == 1) 
    {
        //
        // 1. only one Leave() per pending CloseAsync() will enter here.
        // 2. this can not be set concurently in CloseAsync(), because only one pending CloseAsync() is allowed.
        //
        closeCompletionCallback = (IOperationCompletionCallback*) InterlockedExchangePointer((PVOID volatile *)&m_CloseCompletionCallback, 
                                                                                            NULL);
        //
        // if Closing on Leave(), then the completion pointer must present
        //
        NT_ASSERT(closeCompletionCallback);
                
        eventQueue = m_pAdapter->GetEventQueue();

        ndisStatus = eventQueue->DeferOperationCompletion(closeCompletionCallback,
                                                        NDIS_STATUS_SUCCESS,
                                                        this,
                                                        &m_CloseCallbackEvent);
        if (ndisStatus != NDIS_STATUS_SUCCESS)    
        {
            WFCError("[Port] eventQueue->DeferOperationCompletion(m_PauseCallback) failed %u", ndisStatus);
        } 

        // Lock done
        return TRUE;
    }
            
    return FALSE;
}

