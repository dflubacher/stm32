# STM32F303RE with FreeRTOS
## Example: Mutexes
Semaphores:
- binary
  * Synchronization (task-task, task-interrupt), not message passing
  * Two states:
    * exists (has been 'given')
    * doesn't exist (has never been 'given', or has already been 'taken')
- counting
  * share resources
  * complex synchronization operations

Goals of semaphore:
  * Block or unblock a task
  * Who is irrelevant

## Task priority
> Low priority numbers denote low priority tasks. - [FreeRTOS web site](https://www.freertos.org/RTOS-task-priority.html)

## Comparison
Mutex (`xSemaphoreCreateMutex`):
- Mutex is for protecting a shared resource.
- A mutex is owned by the task that took it.
- When another task attempts to take it, it will fail.
Binary Semaphore (`xSemaphoreCreateBinary()`):
- Binary Semaphore is for synchronization.
- Task B waits for something to happen (requests the semaphore, which doesn't fail).
- Something happens.
- Task A gives semaphore to B so it can execute.

Goals of mutexes:
  * Only the task that blocks another task can unblock it
  * Safeguard against a resource

### References
- [Pomad](https://pomad.cnfm.fr/PoMAD_2021/node/41)
- [Digikey Intro to RTOS 6: Mutex](https://www.digikey.ch/en/maker/projects/introduction-to-rtos-solution-to-part-6-freertos-mutex-example/c6e3581aa2204f1380e83a9b4c3807a6)
    * [youtubue video](https://youtu.be/I55auRpbiTs)

