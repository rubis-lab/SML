
#Default Variables
Libraries = ['rospy','os','cantools']
className = 'can_parser'

#User-Input
nodeName = 'chassis_can_parser'

csvFileName = 'chassis.csv'

Message = [
        ['can_msgs.msg', 'Frame'],
        ['chassis_msgs.msg', 'Chassis'],
    ]

dbcPath = '../DBC/CAN1_Chassis_Simple.dbc'

subscribedTopic = '/can/chassis/rx'

publishMSG = '/chassis'
publishMSGType = 'Chassis'

isCdata = False # True / False
CData = [ ]

anonymous = False # True / False

rate = '50'

isMultipleLogInfo = False # True / False

logInfo = [ ]
