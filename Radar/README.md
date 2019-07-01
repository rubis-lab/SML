# Model
 Name : Delphi ESR 2.5 / Delphi SSR2
 
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
  
  *.msg (All the .msg in msg folder)

...
    
)
```

```c
generate_messages(
  
  DEPENDENCIES
  std_msgs
    
)
```
