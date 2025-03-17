/**
 * @brief thread for producing gas engines
 * 
 * @param ptr expects a pointer to PowertrainProducer
 * 
 * @return NULL
 */
void* gasEngine(void *ptr);

/**
 * @brief thread for producing hybrid engines
 * 
 * @param ptr expects a pointer to PowertrainProducer
 * 
 * @return NULL
 */
void* hybridEngine(void *ptr);

/**
 * @brief thread for titano robot
 * 
 * @param ptr expects a pointer to PoweredChassisProducer
 * 
 * @return NULL
 */
void* titanoRobot(void *ptr);

/**
 * @brief thread for megaforce robot
 * 
 * @param ptr expects a pointer to PoweredChassisProducer
 * 
 * @return NULL
 */
void* megaForceRobot(void *ptr);

/**
 * @brief thread for robomount robot
 * 
 * @param ptr expects a pointer to PoweredChassisConsumer
 * 
 * @return NULL
 */
void* roboMountRobot(void *ptr);