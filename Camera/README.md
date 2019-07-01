# Model
 Name : Mobileye 630
 
 Interface : CAN
 
# How to use
1. Move ***msg*** folder to ***catkin_ws***
2. Open ***CMakelists.txt*** File
3. Add '. msg' context to CMaslists.txt (check for the ***bold***s)
```c
find_package(catkin REQUIRED COMPONENTS
    ...
    message_generation
    ...
)
```

```c
add_message_files(

  ...
  AftermarketLane.msg
  Ahbc.msg
  AhbcGradual.msg
  AwsDisplay.msg	
  FixedFoe.msg	
  Lane.msg	
  LkaLane.msg	
  LkaNumOfNextLaneMarkersReported.msg	
  LkaReferencePoints.msg	
  ObstacleData.msg	
  ObstacleStatus.msg	
  Tsr.msg	
  TsrVisionOnly.msg
  ...
    
)
```

```c
generate_messages(
  
  DEPENDENCIES
  std_msgs
    
)
```
