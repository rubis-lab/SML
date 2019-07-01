
#Default Variables

Libraries = ['rospy','os','cantools']

className = 'autobox_parser'

Message = [
        ['can_msgs.msg', 'Frame'],
        ['low_level_controller_msgs.msg', 'Msg_low_level_controller'],
        ['std_msgs.msg', 'String'],
        ['autobox_msgs.msg','Autobox'],
    ]

# RX_TX Mode Trigger

RX_TX_Mode = True# True / False

#User-Input

# 1. '__init__' Function Variable


nodeName = 'autobox_can_parser' # Generate script file based on node name

csvFileName = 'autobox.csv'

dbcPath = '../DBC/CAN3_IBEO_PC_simple_test_ROS.dbc'

subscribeTopic = [
        ['/can/autobox/rx', 'Frame'],
        ['/low_level_controller_msg', 'Msg_low_level_controller']
    ]

publishTopic = [
        ['/autobox', 'Autobox', 'rx'],
        ['/can/autobox/tx', 'Frame','tx']
]

isCdata = True # True / False
CData = [
        ['max_obstacles', '10'],
        ['obstacle', '[ObstacleInfo()]*10']
]

# 2. 'main' Function Variable

anonymous = False # True / False

rate = '10'

isMultipleLogInfo = False # True / False

logInfo = ['left_lane', 'right_lane']

