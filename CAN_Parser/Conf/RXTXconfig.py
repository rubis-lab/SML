tx_msg_member = [

{       'msgName': 'command_CAN',
        'msgType': ['Automation_On', 'Stop_sig', 'ax_des'],
        'id': '1',
        'dlc': '8',
        'is_error': 'False',
        'is_rtr': 'False',
        'is_extended': 'False',
        'frame_id': 'O'
},

{
        'msgName': 'sas_angle_msg',
        'msgType': ['SAS_Angle_Des'],
        'id': '1600',
        'dlc': '8',
        'is_error': 'False',
        'is_rtr': 'False',
        'is_extended': 'False',
        'frame_id': 'O'
},

{
        'msgName': 'Alive_Counter',
        'msgType': ['Alive_Counter', 'Risk_Flag'],
        'id': '2',
        'dlc': '5',
        'is_error': 'False',
        'is_rtr': 'False',
        'is_extended': 'False',
        'frame_id': 'O'
},

{
        'msgName': 'RT_Heading',
        'msgType': ['H_RT', 'RT_DR_Mode', 'RT_STD'],
        'id': '65',
        'dlc': '5',
        'is_error': 'False',
        'is_rtr': 'False',
        'is_extended': 'False',
        'frame_id': 'O'
}

]












tx_callback = ['def tx_callback(self, data):']