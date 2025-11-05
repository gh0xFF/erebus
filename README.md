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

Created in totalitarian Belarus. During a time of total control over global communications, we lost our basic right to freedom of communication. And the goal of this app is to regain that right.

The entire internet uses the Diffie-Hellman key exchange protocol for key exchange, but it's vulnerable to attack. My idea allows us to bypass this vulnerability.

The app's concept is based on Claude Shannon's work on the mathematical theory of communication. In it, he proved the absolute cryptographic strength of the Vernam cipher, which I use in the app.

Those familiar with this work know that a key can only be used once. I partially solved this problem by imposing some restrictions.

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