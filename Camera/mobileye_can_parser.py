#!/usr/bin/env python
import rospy
import os
import cantools
from can_msgs.msg import Frame
from mobileye_msgs.msg import ObstacleInfo, Mobileye

class can_parser:

    def __init__(self):

        script_dir = os.path.dirname(__file__)
        can_db_path = os.path.join(script_dir, '../DBC/CAN2_TRW.dbc')
        self.db = cantools.database.load_file(can_db_path)

        rospy.Subscriber("/can/mobileye/rx", Frame, self.rx_callback)
        self.rx_pub = rospy.Publisher("/mobileye", Mobileye, queue_size=1)

        self.cdata = Mobileye()
        self.cdata.max_obstacles = 10
        for i in range(self.cdata.max_obstacles):
            self.cdata.obstacle.append(ObstacleInfo())

    def fill_obstacle_info(self, obstacle, info_type, signals):

        if info_type == 0:
            obstacle.longitudinal_distance_std = signals['LongitudinalDistanceSTD']
            obstacle.obstacle_id = signals['ObstacleID']
            obstacle.obstacle_measured = signals['ObstacleMeasured']
            obstacle.obstacle_pos_x = signals['ObstaclePosX']
            obstacle.obstacle_pos_y = signals['ObstaclePosY']
            obstacle.obstacle_valid = signals['ObstacleValid']
            obstacle.obstacle_vel_x = signals['ObstacleVelX']
        elif info_type == 1:
            obstacle.blinker_info = signals['BlinkerInfo']
            obstacle.inverse_ttc = signals['Inverse_TTC']
            obstacle.obstacle_age = signals['ObstacleAge']
            obstacle.obstacle_brake_lights = signals['ObstacleBrakeLights']
            obstacle.obstacle_lane = signals['ObstacleLane']
            obstacle.obstacle_status = signals['ObstacleStatus']
            obstacle.obstacle_type = signals['ObstacleType']
            obstacle.obstacle_width = signals['ObstacleWidth']
        elif info_type == 2:
            obstacle.angle_std = signals['AngleSTD']
            obstacle.obstacle_angle_left = signals['ObstacleAngleLeft']
            obstacle.obstacle_angle_rate = signals['ObstacleAngleRate']
            obstacle.obstacle_angle_right = signals['ObstacleAngleRight']
            obstacle.obstacle_accel_x = signals['Obstacle_Accel_X']

    def fill_lane_info(self, lane, info_type, signals):
        if info_type == 0:
            lane.curvature = signals['Curvature']
            lane.curvature_derivative = signals['Curvature_Derivative']
            lane.lane_location_type = signals['Lane_Location_Type']
            lane.lane_type = signals['Lane_Type']
            lane.position = signals['Position']
            lane.quality = signals['Quaility']
        elif info_type == 1:
            lane.heading_angle = signals['Heading_Angle']
            lane.lane_crossing = signals['Lane_Crossing']
            lane.lane_view_range_end = signals['Lane_View_Range_End']
            lane.lane_view_range_start = signals['Lane_View_Range_Start']
            lane.lane_mark_color = signals['Lane_mark_color']
            lane.position_std = signals['Position_STD']
            lane.tlc = signals['TLC']
            lane.width_marking = signals['Width_marking']

    def rx_callback(self, data):
        try:
            signals = self.db.decode_message(data.id, data.data)
        except KeyError:
            return

        if data.id >= 1838 and data.id <= 1847:
            lane_message_id = data.id - 1838
            (lane_id,info_type) = divmod(lane_message_id,2)
            if lane_id == 0:
                self.fill_lane_info(self.cdata.left_lane,info_type,signals)
            elif lane_id == 1:
                self.fill_lane_info(self.cdata.right_lane,info_type,signals)
            elif lane_id == 3:
                self.fill_lane_info(self.cdata.next_left_lane,info_type,signals)
            elif lane_id == 4:
                self.fill_lane_info(self.cdata.next_right_lane,info_type,signals)
        elif data.id >= 1793 and data.id <= 1822:
            obstacle_message_id = data.id - 1793
            (obstacle_id,info_type) = divmod(obstacle_message_id,3)
            self.fill_obstacle_info(self.cdata.obstacle[obstacle_id],info_type,signals)

def main():

    cp = can_parser()
    rospy.init_node('mobileye_can_parser', anonymous=False)

    rate = rospy.Rate(30) # 30hz
    while not rospy.is_shutdown():
	cp.cdata.header.stamp = rospy.Time.now()
        cp.rx_pub.publish(cp.cdata)
        rate.sleep()

if __name__ == '__main__':
	main()
