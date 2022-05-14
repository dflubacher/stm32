# STM32F303RE with FreeRTOS
## Example: Binary Semaphore
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

## Functions
- `xSemaphoreGive(binary_semaphore);` yields control to equal or higher priority tasks.

### References
- [Pomad](https://pomad.cnfm.fr/PoMAD_2021/node/39)
- [Digikey Intro to RTOS 7: Semaphore](https://www.digikey.ch/en/maker/projects/introduction-to-rtos-solution-to-part-7-freertos-semaphore-example/51aa8660524c4daba38cba7c2f5baba7)
    * [youtube video](https://youtu.be/5JcMtbA9QEE)

