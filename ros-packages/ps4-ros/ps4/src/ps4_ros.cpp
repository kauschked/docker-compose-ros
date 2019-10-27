#include <string>
#include <ros/ros.h>
#include <sensor_msgs/Joy.h>
#include <geometry_msgs/Twist.h>

#include <math.h>       /* tan */

class PS4_ROS {

public:

    const double pi = 3.14;
    const double maxDegree = pi/6;
    const double minTurningRadius = 1.156;

    /**
     * @brief      { PS4 to TWIST MESSAGES }
     *
     */
    PS4_ROS(ros::NodeHandle &n) {
        // get ros param
        ros::NodeHandle private_nh("~");
        private_nh.param("scale_linear", this->scale_linear, 1.0);
        private_nh.param("scale_angular", this->scale_angular, 1.0);

        // Publish to vel_cmd
        private_nh.param<std::string>("pub_topic", this->pubName, "/pioneer3dx/cmd_vel");

        this->chat = n.advertise<geometry_msgs::Twist>(pubName, 1000);

        // Subscribe to ps4 controller
        this->sub = n.subscribe<sensor_msgs::Joy>("/joy", 10, &PS4_ROS::subscribePS4, this);

        this->joyMessageReceived = false;

        // forward velocity
        this->maxVel = this->scale_linear;
        // backward velocity
        this->maxVelR = this->scale_linear * -1;

        ROS_INFO("maxVelR: %f", this->maxVelR);

        ROS_INFO("scale_linear set to: %f", this->scale_linear);
        ROS_INFO("scale_angular set to: %f", this->scale_angular);
        ROS_INFO("PS4_ROS initialized");
    }

    ~PS4_ROS() {
        // std::cout << "Destroy the pointer" << std::endl;
    }

    void run() {
        if (!this->joyMessageReceived) {
            this->leftStickY = 0.0;
        }
        this->joyMessageReceived = false;
        this->publishTwistMsg();
    }

    void publishTwistMsg() {
        geometry_msgs::Twist msg;
        msg.linear.x = 0.0;
        msg.linear.y = 0.0;
        msg.linear.z = 0.0;

        msg.angular.x = 0.0;
        msg.angular.y = 0.0;
        msg.angular.z = 0.0;

        double speed = this->scale_linear * this->leftStickY;
        double angle = this->scale_angular * this->rightStickX;

        // Scale speed if r1 is pressed
        if ( this->r1 ) {
            speed *= 0.5;
        }

        msg.angular.z = angle * (-1);

        msg.linear.x = speed;
        this->chat.publish(msg);
    }

    void subscribePS4(const sensor_msgs::Joy::ConstPtr &joy) {
        this->buttonX = joy->buttons[0];
        this->buttonO = joy->buttons[1];
        this->buttonTr = joy->buttons[2];
        this->buttonSq = joy->buttons[3];
        this->buttonTouch = joy->buttons[13];
        this->l1 = joy->buttons[4];
        this->r1 = joy->buttons[5];

        this->arrowsX = joy->axes[9];
        this->arrowsY = joy->axes[10];
        this->l2 = joy->axes[2];
        this->r2 = joy->axes[5];

        this->leftStickX = joy->axes[0];
        this->leftStickY = joy->axes[1];
        this->rightStickX = joy->axes[3];
        this->rightStickY = joy->axes[4];

        this->joyMessageReceived = true;

        //printRaw();
    }

    void printRaw()
    {

        ROS_INFO("#####################################");
        ROS_INFO("Squared Button pressed: %i", this->buttonSq);
        ROS_INFO("X Button pressed: %i", this->buttonX);
        ROS_INFO("O Button pressed: %i", this->buttonO);
        ROS_INFO("Triangel Button pressed: %i", this->buttonTr);
        ROS_INFO("Left/Right Button pressed: %i", this->arrowsX);
        ROS_INFO("Down/Up Button pressed: %i", this->arrowsY);
        ROS_INFO("Touch Button pressed: %i", this->buttonTouch);
        ROS_INFO("L1: %i", this->l1);
        ROS_INFO("R1: %i", this->r1);
        ROS_INFO("L2: %f", this->l2);
        ROS_INFO("R2: %f", this->r2);
        ROS_INFO("Left Stick Y: %f", this->leftStickY);
        ROS_INFO("Left Stick X: %f", this->leftStickX);
        ROS_INFO("Right Stick Y: %f", this->rightStickY);
        ROS_INFO("Right Stick X: %f", this->rightStickX);
        ROS_INFO("##################################### \n");
    }

private:
    ros::Publisher chat;
    ros::Subscriber sub;

    /* raw data */
    double leftStickY{0.0}, leftStickX{0.0}, rightStickY{0.0}, rightStickX{0.0}, l2{0.0}, r2{0.0};
    int arrowsX{0}, arrowsY{0}, buttonSq{0}, buttonX{0}, buttonO{0}, buttonTr{0},
        buttonTouch{0}, l1{0}, r1{0};

    /* rosparams */
    double scale_linear, scale_angular;
    std::string pubName;

    bool joyMessageReceived;

    double maxVel, maxVelR;
};

int main(int argc, char **argv) {
    ros::init(argc, argv, "PS4_ROS");
    ros::NodeHandle n;

    // create ps4_ros object
    PS4_ROS *ps4_ros = new PS4_ROS(n);

    ros::Rate loop_rate(10);
    while(ros::ok())
    {
        ps4_ros->run();
        ros::spinOnce();
        loop_rate.sleep();
    }

    delete ps4_ros;
    return 0;
}
