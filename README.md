```
   ('-.  _  .-')     ('-. .-. .-')                 .-')    
 _(  OO)( \( -O )  _(  OO)\  ( OO )               ( OO ).  
(,------.,------. (,------.;-----.\  ,--. ,--.   (_)---\_) 
 |  .---'|   /`. ' |  .---'| .-.  |  |  | |  |   /    _ |  
 |  |    |  /  | | |  |    | '-' /_) |  | | .-') \  :` `.  
(|  '--. |  |_.' |(|  '--. | .-. `.  |  |_|( OO ) '..`''.) 
 |  .--' |  .  '.' |  .--' | |  \  | |  | | `-' /.-._)   \ 
 |  `---.|  |\  \  |  `---.| '--'  /('  '-'(_.-' \       / 
 `------'`--' '--' `------'`------'   `-----'     `-----'  
```

Created in totalitarian Belarus. During the times of total control of communication with the quarantine of the world, we were participants of our basic rights, one of which was freedom of correspondence. And the goal of this application is to return this right.

The entire Internet uses the Diffie-Hellman key exchange protocol, which has enormous power in video with the possibility of attack. My idea allows you to avoid this vulnerability.

The idea of ​​the application is based on the work of Claude Chennno about mathematical communication. In it, he proved the absolute cryptographic resistance of the Vernam cipher, which I use in the application.

Those who are already familiar with this activity know that the key can only be used once. I partially managed to solve this problem with some restrictions.

1) create a file with questions, which is described in [example](./examples/template.txt)
2) we send to the interlocutor through an unreliable channel
3) fill in the message file [example](./examples/msg.txt)
4) the program reads the file, extracts the responses and uses it as a seed to generate the key. One of the arguments to cli is the message order. It also introduces chaos into the generator. At the output we get [encrypted file](./examples/encrypted_message.txt).
5) send an encrypted file to the interlocutor via an insecure channel
6) This is where the most interesting thing happens, the interlocutor fills out a file [template](./examples/template.txt) the same values.
7) the interlocutor calls the application and transfers the file to it [example](./examples/template.txt), encrypted [encrypted file](./examples/encrypted_message.txt) and offset (positive integer). Everything works similarly to point 4 and as a result we get a decrypted file containing a decrypted message with garbage.


## example of message encryption
```
erebus -t template.txt -m msg.txt -o 1 -e
```

## example of message decryption
```
erebus -t template.txt -m encrypted_msg.txt -o 1 -d
```

## ! IMPORTANT NOTES: use every time new offset to encrypt message