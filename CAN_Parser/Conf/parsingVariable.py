# Environment & Libraries

envHeader = ['#!/usr/bin/env python']
msgHeaderFrom = ['from']
msgHeaderImport = ['import']

# Class
classHeader = ['class']

# Function
initFunction = ['def __init__(self):']
scriptDIR = ['script_dir = os.path.dirname(__file__)']
canDBPath = ['can_db_path = os.path.join']
script_dir = ['script_dir']

selfDB = ['self.db = cantools.database.load_file(can_db_path)']
subscriber = ['rospy.Subscriber']
rx_publisher = ['self.rx_pub = rospy.Publisher']
tx_publisher = ['self.tx_pub = rospy.Publisher']
rx_data = ['self.rx_data']
tx_data = ['self.tx_data']


callBackFunc = ['self.rx_callback']

queueSize = ['queue_size=1']

cdata = ['self.cdata']

rx_callback = ['def rx_callback(self, data):']
tx_callback = ['def tx_callback(self, data):']

dataVariable = ['data.']
tryVariable = ['try:']

exceptKeyError = ['except KeyError:']
decode_message = ['signals = self.db.decode_message(data.id, data.data)']
returnVariable = ['return']

#Body
conditional_if = ['if data.id == ']
conditional_elif = ['elif data.id == ']
variable = ['self.data.']
signals_begin = ['signals', '[', '\'']
signals_end = ['\'', ']', '\n']

#Main
mainFunction = ['def main():']
classVariable = ['cp = ']
initNode = ['rospy.init_node']
anonymousVariable = ['anonymous']
rateVariable = ['rate = rospy.Rate']

shutdownCondition = ['while not rospy.is_shutdown():']
headerStamp = ['cp.cdata.header.stamp = rospy.Time.now()']

logInfoVariable = ['rospy.loginfo']
cpCdata = ['cp.cdata']
cpRXdata = ['cp.rx_data']

publishVariable = ['cp.rx_pub.publish']
sleepVariable = ['rate.sleep()']

#Main Conditional
mainIF = ['if __name__ == ', '\'', '__main__', '\'', ':']
mainVariable = ['main()']

#TX Part
canMSGVariable = ['can_msg_']
encodeVariable = ['self.db.encode_message']
txDataVariable = ['data']
txIdVariable = ['id']
txDlcVariable = ['dlc']
txIsErrorVariable = ['is_error']
txIsRtrVariable = ['is_rtr']
txIsExtendedVariable = ['is_extended']
txHeaderFrame = ['header.frame_id']
txHeaderStamp = ['self.tx_data.header.stamp = rospy.Time.now()']
txPublish = ['self.tx_pub.publish(self.tx_data)']

# basicVariable
space = [' ']
comma = [', ']
dot = ['.']
equal = [' = ']
nospaceEqual = ['=']
singleQuote = ['\'']
doubleQuote = ['\"']

colon = [':']
indentation = ['    ']
doubleIndentation = ['        ']
tripleIndentation = ['            ']

parenthesisOpen = ['(']
parenthesisClose = [')']

bracketOpen = ['{']
bracketClose = ['}']

squarebracketOpen = ['[']
squarebracketClose = [']']