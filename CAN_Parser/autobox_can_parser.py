#!/usr/bin/env python
import rospy
import os
import cantools
from can_msgs.msg import Frame
from low_level_controller_msgs.msg import Msg_low_level_controller
from std_msgs.msg import String
from autobox_msgs.msg import Autobox

class autobox_parser:

    def __init__(self):

        script_dir = os.path.dirname(__file__)
        can_db_path = os.path.join(script_dir, '../DBC/CAN3_IBEO_PC_simple_test_ROS.dbc')
        self.db = cantools.database.load_file(can_db_path)

        rospy.Subscriber("/can/autobox/rx", Frame, self.rx_callback)
        rospy.Subscriber("/low_level_controller_msg", Msg_low_level_controller, self.rx_callback)
        self.rx_pub = rospy.Publisher("/autobox", Autobox, queue_size=1)
        self.tx_pub = rospy.Publisher("/can/autobox/tx", Frame, queue_size=1)

        self.rx_data = Autobox()
        self.tx_data = Frame()

    def rx_callback(self, data):
        try:
            signals = self.db.decode_message(data.id, data.data)

        except KeyError:
            return

        if data.id == 65:
            self.rx_data.rt_std = signals['RT_STD']
            self.rx_data.h_rt = signals['H_RT']
        elif data.id == 4:
            self.rx_data.can_autobox_alive = signals['CAN3_Alive']
            self.rx_data.h_dr = signals['Yaw_DR']
        elif data.id == 265:
            self.rx_data.x_dr = signals['X_DR']
            self.rx_data.y_dr = signals['Y_DR']
        elif data.id == 272:
            self.rx_data.x_rt = signals['X_RT']
            self.rx_data.y_rt = signals['Y_RT']
        elif data.id == 17:
            self.rx_data.automation_autobox = signals['Automation_Autobox']

    def tx_callback(self, data):
        try:
            command_CAN = {}
            command_CAN['Automation_On'] = data.Automation_On
            command_CAN['Stop_sig'] = data.Stop_sig
            command_CAN['ax_des'] = data.ax_des
            can_msg_command_CAN = self.db.encode_message(1, command_CAN)
            self.tx_data.data = can_msg_command_CAN
            self.tx_data.id = 1
            self.tx_data.dlc = 8
            self.tx_data.is_error = False
            self.tx_data.is_rtr = False
            self.tx_data.is_extended = False
            self.tx_data.header.frame_id = "O"
            self.tx_data.header.stamp = rospy.Time.now()
            self.tx_pub.publish(self.tx_data)

            sas_angle_msg = {}
            sas_angle_msg['SAS_Angle_Des'] = data.SAS_Angle_Des
            can_msg_sas_angle_msg = self.db.encode_message(1600, sas_angle_msg)
            self.tx_data.data = can_msg_sas_angle_msg
            self.tx_data.id = 1600
            self.tx_data.dlc = 8
            self.tx_data.is_error = False
            self.tx_data.is_rtr = False
            self.tx_data.is_extended = False
            self.tx_data.header.frame_id = "O"
            self.tx_data.header.stamp = rospy.Time.now()
            self.tx_pub.publish(self.tx_data)

            Alive_Counter = {}
            Alive_Counter['Alive_Counter'] = data.Alive_Counter
            Alive_Counter['Risk_Flag'] = data.Risk_Flag
            can_msg_Alive_Counter = self.db.encode_message(2, Alive_Counter)
            self.tx_data.data = can_msg_Alive_Counter
            self.tx_data.id = 2
            self.tx_data.dlc = 5
            self.tx_data.is_error = False
            self.tx_data.is_rtr = False
            self.tx_data.is_extended = False
            self.tx_data.header.frame_id = "O"
            self.tx_data.header.stamp = rospy.Time.now()
            self.tx_pub.publish(self.tx_data)

            RT_Heading = {}
            RT_Heading['H_RT'] = data.H_RT
            RT_Heading['RT_DR_Mode'] = data.RT_DR_Mode
            RT_Heading['RT_STD'] = data.RT_STD
            can_msg_RT_Heading = self.db.encode_message(65, RT_Heading)
            self.tx_data.data = can_msg_RT_Heading
            self.tx_data.id = 65
            self.tx_data.dlc = 5
            self.tx_data.is_error = False
            self.tx_data.is_rtr = False
            self.tx_data.is_extended = False
            self.tx_data.header.frame_id = "O"
            self.tx_data.header.stamp = rospy.Time.now()
            self.tx_pub.publish(self.tx_data)

        except KeyError:
            return

def main():

    cp = autobox_parser()
    rospy.init_node('autobox_can_parser', anonymous=False)

    rate = rospy.Rate(10)
    while not rospy.is_shutdown():
	cp.cdata.header.stamp = rospy.Time.now()
        rospy.loginfo(cp.rx_data)
        cp.rx_pub.publish(cp.rx_data)
        rate.sleep()

if __name__ == '__main__':
	main()
