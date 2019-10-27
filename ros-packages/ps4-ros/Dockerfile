FROM ros:melodic-ros-base

WORKDIR /root

RUN apt-get update
RUN apt-get install -y iputils-ping vim expect ros-melodic-joy

RUN mkdir -p catkin_ws/src && cd catkin_ws/src

RUN git clone https://github.com/solbach/ps4-ros.git catkin_ws/src/ps4

RUN . /opt/ros/$ROS_DISTRO/setup.sh && cd catkin_ws/src && catkin_init_workspace && cd .. && catkin_make clean && catkin_make -DCATKIN_ENABLE_TESTING=False -DCMAKE_BUILD_TYPE=Release && catkin_make install

RUN echo "source /opt/ros/melodic/setup.bash" >> ~/.bashrc
RUN echo "source /root/catkin_ws/devel/setup.sh" >> ~/.bashrc
