# TycheBot
Trabajo del Laboratorio de Robótica y Automática del grupo Tyche

## Paquetes necesarios
- gazebo_ros_control: paquete para usar Gazebo con ROS.
- robotnik_sensors: paquete con descripciones estándar de diversos sensores para ROS y Gazebo.
- gmapping: paquete de SLAM.
- navigation: paquete de navegación: mapas de coste, generación de trayectorias, control, etc.

## Descripción detallada

### Paquete tychebot_description
Este paquete contiene los modelos 3D y archivos básicos del robot, así como launchfiles para visualización en Rviz y Gazebo.
#### Launchfiles
- tychebot_rviz.launch: lanza el robot en el visualizador Rviz.
- tychebot_gazebo.launch: lanza el robot y los sensores en Gazebo.
- tychebot_labrob.launch: lanza el robot y los sensores directamente sobre el mundo con los objetos, y la visualización en Rviz.
- tychebot_slam.launch: lanza el nodo de SLAM (pendiente de mover a tychebot_control).
- tychebot_slam.xml: parámetros de SLAM.

### Paquete tychebot_control
Este paquete contiene las utilidades de control y navegación (actualmente navegación 2D y SLAM).
#### Launchfiles
- amcl.launch: lanza el nodo amcl para su uso en navegación 2D con mapa fijo.
- tychebot_2dnav.launch: lanza el navigation stack y carga los archivos de configuración pertinentes.
- tychebot_control.launch: lanza el control de las ruedas y la cámara ptz.

