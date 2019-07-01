# Model
 Name : Mobileye 630
 
 Interface : CAN
 
# How to use
1. Move ' .msg' file to 'catkin_ws'
2. Open CMakelists.txt File
3. Add '. msg' context to CMaslists.txt
```c
find_package(catkin REQUIRED COMPONENTS
    
)
```

```c
add_message_files(

  FILES
    
)
```

```c
generate_messages(
  
  DEPENDENCIES
  std_msgs
    
)
```
