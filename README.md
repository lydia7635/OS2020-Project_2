# OS2020-Project_2
Please check report.pdf to see more.

### Execution
To use it, please:
1. change to super user  
2. execute `./compile.sh` to compile codes and install modules  
3. follow the input instructions in the spec, i.e.  
`./master 1 file1_in mmap`  
`./slave 1 file1_out fcntl 127.0.0.1`  
Make sure that you are under the path `./user_program` when you execute user programs.  
Though the execution order of user program "master" and "slave" does not matter , it is suggested to execute "master" first to get more precise transmission time.
