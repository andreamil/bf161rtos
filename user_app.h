#ifndef USER_APP_H
#define	USER_APP_H

#include "os_config.h"
#include "types.h"


void config_app(void);

// Tarefas do drone
TASK task_motors(void);
TASK task_sensors(void);
TASK task_battery(void);
TASK task_central(void);


#endif	/* USER_APP_H */

