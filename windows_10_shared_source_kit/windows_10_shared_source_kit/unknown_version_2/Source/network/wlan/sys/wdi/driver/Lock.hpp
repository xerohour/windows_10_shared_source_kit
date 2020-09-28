#pragma once

/* 
 * This is the structure we use to maintain the counts for number
 * of enters and leaves from the gate
 */
typedef union _WFC_COUNTED_GATE_REFERENCE_OBJECT {
    struct {
        ULONG Closed : 1;           // least significant bit
        ULONG ReferenceCount : 30;  // mid 30 bits
        ULONG Overflow : 1;         // most significant bit
    };
    LONG Value;                     // ReferenceCount * 2 + Closed
} WFC_COUNTED_GATE_REFERENCE_OBJECT, *PWFC_COUNTED_GATE_REFERENCE_OBJECT;

/* Initialize the gate as closed
 * Closed = TRUE, ReferenceCount = 0;
 */
#define WFC_GRO_VALUE_INIT_CLOSED 1

/* Initialize the gate as open
 * Closed = FALSE, ReferenceCount = 1;
 */
#define WFC_GRO_VALUE_INIT_OPEN 2

class CAdapter;
class IOperationCompletionCallback;
/*
 * This class maintains a synchronous gate object. When the gate 
 * is closed or is being closed, all Enters will fail. While the 
 * gate is open, all the Enters will be counted and the gate 
 * will not be allowed to be Closed again until all those Enters 
 * have left.
 */
class CWFCExclusiveAsyncGate
{
    private:
        // Reference object keeping track of everything
        WFC_COUNTED_GATE_REFERENCE_OBJECT   ReferenceObject;
        volatile IOperationCompletionCallback *m_CloseCompletionCallback;
        Event m_CloseCallbackEvent;
        CAdapter *m_pAdapter;

    public:
        /* 
         * Constructor initializes the gate as closed 
         */
        //If the constructor without pAdapter is used Initalize MUST be called
        CWFCExclusiveAsyncGate() :
            m_CloseCompletionCallback(NULL),
            m_CloseCallbackEvent(TRUE),
            m_pAdapter(NULL)
        {
            //
            // ReferenceObject.Closed = TRUE;
            // ReferenceObject.ReferenceCount = 0;
            //
            ReferenceObject.Value = WFC_GRO_VALUE_INIT_CLOSED;
        }
        CWFCExclusiveAsyncGate(_In_ CAdapter *pAdapter):
            m_CloseCompletionCallback(NULL),
            m_CloseCallbackEvent(TRUE),
            m_pAdapter(pAdapter)
        {
            //
            // ReferenceObject.Closed = TRUE;
            // ReferenceObject.ReferenceCount = 0;
            //
            ReferenceObject.Value = WFC_GRO_VALUE_INIT_CLOSED;
        }

        ~CWFCExclusiveAsyncGate()
        {
            // Ensure we are closed
            NT_ASSERT(ReferenceObject.Value == WFC_GRO_VALUE_INIT_CLOSED);
        }

        void Initialize(_In_ CAdapter *pAdapter)
        {
            m_pAdapter = pAdapter;
        }

        /* 
         * Returns current close status of the gate
         *
         * \return 
         *      TRUE if gate is currently closed
         *      FALSE otherwise
         */
        BOOLEAN IsClosed()
        {
            return (BOOLEAN) (ReferenceObject.Value == WFC_GRO_VALUE_INIT_CLOSED);
        }
    
        /* 
         * Closes a gate. If there are pending Enters into the gate,
         * the close will complete asynchronously. 
         *
         * \note Once Close is called, it CANNOT be called again
         *       until its open again
         *
         * \return 
         *      TRUE if close is complete.  
         *      FALSE if completion will be signalled asynchronously 
         *          (from a Leave call).
         */
        BOOLEAN CloseAsync(
            _In_opt_ IOperationCompletionCallback * completionCallback);

        /* 
         * Opens a gate. 
         */
        VOID Open()
        {
            WFC_COUNTED_GATE_REFERENCE_OBJECT NewValue;

            //
            // ReferenceObject.Closed = FALSE;     // Unmark Closed
            // ReferenceObject.ReferenceCount++;   // Increment ReferenceCount
            // The former is achieved by subtracting 1 from the reference object
            // value and the latter by adding 1 from it.  We increment by 1 to
            // achieve the combined effect!
            //
            NewValue.Value = InterlockedIncrement(&ReferenceObject.Value);

            //
            // Closed should have been marked.  An attempt to reinitialize an
            // initialized object would flip this bit resulting in an assert.
            //
            NT_ASSERT(!NewValue.Closed);
        }

        /* 
         * Checks and allows specified number of "counters" to Enter the gate. 
         * If the gate is already Closed, Enter will fail and the counter
         * will not be added
         *
         * \return 
         *      TRUE if Enter was successful
         *      FALSE if gate is closed and so Enter failed
         */        
        BOOLEAN Enter()
        {
            WFC_COUNTED_GATE_REFERENCE_OBJECT Snapshot, Old, NewValue;
     
            do 
            {
                //
                // Take a snapshot of the current value.
                //
                Snapshot.Value = ReferenceObject.Value;

                //
                // Fail if Closed
                //
                if (Snapshot.Closed) 
                {
                    return FALSE;
                }

                //
                // NewValue.Closed = Snapshot.Closed;
                // NewValue.ReferenceCount = Snapshot.ReferenceCount + Count;
                // This is achieved by adding 2*Count to the snapshot value.
                //
                NewValue.Value = Snapshot.Value + 2;

                //
                // Do the number of outstanding references exceed our limit?
                // That would cause the most significant bit to get set!
                //
                NT_ASSERT(!NewValue.Overflow);

                //
                // Update ReferenceCount, but only if the current value is the
                // same as that obtained in the snapshot.  Hence this operation
                // will fail if the reference object has been marked Closed.
                //
                Old.Value = InterlockedCompareExchange(&ReferenceObject.Value, 
                                                    NewValue.Value, 
                                                    Snapshot.Value);

                //
                // Repeat until the ReferenceCount is successfully updated or
                // the Closed bit is observed to be set.
                //
            } while (Old.Value != Snapshot.Value);

            // Enter successful
            return TRUE;
        }

        /*
         * Allows specified number of "counters" to leave. If 
         * a Close has been pending and all Enters have exited, the
         * Close can now be completed
         *
         * \return
         *      TRUE to signal completion of a previous close request.
         *      FALSE otherwise.
         */
        BOOLEAN Leave();
};
