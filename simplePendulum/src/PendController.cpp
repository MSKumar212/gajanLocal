#include "PendController.hpp"
#include <ocl/Component.hpp>
ORO_CREATE_COMPONENT_TYPE();
ORO_LIST_COMPONENT_TYPE(simplePendulum::PendController);
//ORO_CREATE_COMPONENT(simplePendulum::PendController);

using namespace std;

namespace simplePendulum
{

    using namespace RTT;
    using namespace KDL;
    using namespace std;

    PendController::PendController(string name)
        : TaskContext(name,PreOperational)
    {

    	//Hardcoding values. (Herman Alert!)
    	originX = 0.150;
    	originY = 0.0;
    	originZ = 0.830;

    	Xr = 0.08;
    	Yr = 0.08;

    	s_t = std::vector<double>(8, 0.0);
    	s_tm1 = std::vector<double>(8, 0.0); //not used now
    	double inputArr[] = { 10.7359,   34.3973,   -2.6273,   -1.2995, 10.7359,   34.3973,   -2.6273,   -1.2995};
    	gainLQR = std::vector<double>(inputArr,inputArr+sizeof(inputArr)/sizeof(double));

    	u_t = std::vector<double>(2, 0.0);
    	u_tm1 = std::vector<double>(2, 0.0);//not used now
    	dT = 0.02;

    	xr_dot = 0.0;
    	yr_dot = 0.0;

    	xr_tm1 = 0.0;
    	yr_tm1 = 0.0;

        //Adding Ports
    	this->addPort("CartesianPoseOutput", m_position_desi);
    	this->addPort("pendPos_inputPort", pendPos_inputPort);

    	this->addOperation("moveToInitialPose",&PendController::moveToInitialPose,this,OwnThread);

    }

    PendController::~PendController()
    {

    }

    bool PendController::configureHook()
    {
		return true;
    }

    bool PendController::startHook()
    {

		return true;

    }

    void PendController::updateHook()
    {
    	//controller (spits out xr, yr for the robot
    	for(int i=0; i<4;i++){
    		u_t[0] += gainLQR[i]*s_t[i];
    	}
    	for(int i=4; i<8;i++){
			u_t[1] += gainLQR[i]*s_t[i];
		}

    	//updating position and velocity (order matters !)
    	s_t[3] = s_t[3] + 0.5*s_t[2]*(s_t[2] + dT * u_t[0]); // xr
    	s_t[2] = s_t[2] + dT * u_t[0] ; // xr_dot;

    	s_t[7] = s_t[7] + 0.5*s_t[6]*(s_t[6] + dT * u_t[1]); // yr
    	s_t[6] = s_t[6] + dT * u_t[1] ; // yr_dot;

    	xr = s_t[3];
    	yr = s_t[7];

    	//generate random xr, yr;
        //xr = -Xr + 2*Xr*(double)rand()/(double)RAND_MAX;
        //yr = -Yr + 2*Yr*(double)rand()/(double)RAND_MAX;

        if(abs(xr)>Xr || abs(yr)>Yr){
        	cout << "PendController::Commanded Value out of Range" << endl;
        		cout << "	xr = " << xr << endl;
        		cout << "	yr = " << yr << endl;
        	this->stop();
        }else{
			//Send Pose to Robot
			geometry_msgs::Pose pose;

			//Always point up
			pose.orientation.x = 0.0;
			pose.orientation.y = 0.0;
			pose.orientation.z = 0.0;
			pose.orientation.w = 1.0;

			pose.position.x = originX + xr;
			pose.position.y = originY + yr;
			pose.position.z = originZ;

			m_position_desi.write(pose);
        }

    }

    void PendController::stopHook()
    {
    }

    void PendController::cleanupHook()
    {
    }

    bool PendController::moveToInitialPose(){
		geometry_msgs::Pose pose;

		// Always point up
		pose.orientation.x = 0.0;
		pose.orientation.y = 0.0;
		pose.orientation.z = 0.0;
		pose.orientation.w = 1.0;

		pose.position.x = originX;
		pose.position.y = originY;
		pose.position.z = originZ;

		m_position_desi.write(pose);

		return true;
    }
}//namespace
