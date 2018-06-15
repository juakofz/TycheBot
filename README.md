# TycheBot
Trabajo del Laboratorio de Robótica y Automática del grupo Tyche

## Paquetes necesarios
- gazebo_ros_control: paquete para usar Gazebo con ROS.
- robotnik_sensors: paquete con descripciones estándar de diversos sensores para ROS y Gazebo.
- gmapping: paquete de SLAM.
- navigation: paquete de navegación: mapas de coste, generación de trayectorias, control, etc.

## Descripción detallada

### Paquete tychebot_common
Este paquete contiene los launchfiles necesarios para el control del robot.

#### Launchfiles
Están separados en carpetas según su función. Se puede lanzar el robot desde tychebot_complete.launch utilizando los parámetros incluidos.

Para lanzar el robot con en willow garage con navegación en mapa fijo, podemos utilizar:
```
roslaunch tychebot_common/launch/tychebot_complete.launch move_base_enabled:=true amcl_and_mapserver_enabled:=true
```

### Paquete tychebot_description
Este paquete contiene los modelos 3D y archivos URDF de descripción del robot.
