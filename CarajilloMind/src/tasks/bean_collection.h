#ifndef BEAN_COLLECTION_H
#define BEAN_COLLECTION_H

#include "../hardware/sensors.h"
#include "../hardware/mechanisms.h"
#include "../config.h"

class BeanCollection {
public:
    void init();
    
    // Recolección por nivel
    void collectFromLevel(TreeLevel level);
    void collectAllBeansFromTree();
    
    // Detección y clasificación
    BeanType detectBeanAtPosition();
    bool isBeanPresent();
    
    // Secuencia de recolección
    void grabBean();
    void storeBean(BeanType type);
    
    // Depositar en contenedores
    void depositAllBeans();
    
    // Contadores
    int getRipeBeansCollected();
    int getOverripeBeansCollected();
    void resetCounters();
    
private:
    int ripeCount;
    int overripeCount;
    
    void executePickSequence(BeanType type);
    void waitForBeanInPosition();
};

extern BeanCollection beanCollection;

#endif