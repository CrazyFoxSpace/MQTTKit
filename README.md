# MQTTKit

MQTTKit is a modern event-driven Objective-C library for [MQTT 3.1][mqtt].

It uses [Mosquitto 1.2.3](http://mosquitto.org) library.

## Usage

Import the `MQTTKit.h` header file

```objc
#import <MQTTKit.h>
```

### Send a Message

```objc
// create the client with a unique client ID
NSString *clientID = ...
MQTTClient *client = [[MQTTClient alloc] initWithClientId:clientID];

// connect to the MQTT server
[self.client connectToHost:@"iot.eclipse.org" 
         completionHandler:^(NSUInteger code) {
    if (code == ConnectionAccepted) {
        // when the client is connected, send a MQTT message
        [self.client publishString:@"Hello, MQTT"
                           toTopic:@"/MQTTKit/example"
                           withQos:AtMostOnce
                            retain:NO
                 completionHandler:^(int mid) {
            NSLog(@"message has been delivered");
        }];
    }
}];

```

### Subscribe to a Topic and Receive Messages

```objc

// define the handler that will be called when MQTT messages are received by the client
[self.client setMessageHandler:^(MQTTMessage *message) {
    NSString *text = [message.payloadString];
    NSLog(@"received message %@", text);
}];

// connect the MQTT client
[self.client connectToHost:@"iot.eclipse.org"
         completionHandler:^(MQTTConnectionReturnCode code) {
    if (code == ConnectionAccepted) {
        // when the client is connected, subscribe to the topic to receive message.
        [self.client subscribe:@"/MQTTKit/example"
         withCompletionHandler:nil];
    }
}];
```

### Disconnect from the server

```objc
[self.client disconnectWithCompletionHandler:^(NSUInteger code) {
    // The client is disconnected when this completion handler is called
    NSLog(@"MQTT client is disconnected");
}];
```

### TLS/SSL support

__Mosquitto support TLS/SSL by OpenSSL but it's removed in iOS. So you need compile OpenSSL for iOS Devices (iPhone, iPad, iPod Touch, AppleTV)  by yurself. -> [OpenSSL-for-iPhone](https://github.com/x2on/OpenSSL-for-iPhone)

* Mutual authentication

```objc
self.client.tlsCafile = [[NSBundle mainBundle] pathForResource:@"server" ofType:@"crt"];
    
[self.client connectWithCompletionHandler:^(MQTTConnectionReturnCode code) {
    if (code == 0)
    {
        NSLog(@"Connect Success");
    }
    else
    {
        NSLog(@"Connect Fail! State Code is %lu", (unsigned long)code);
    }
}];
```

* One way authentication

```objc
client.tlsCerPath = [[NSBundle mainBundle] pathForResource:@"client" ofType:@"crt"];
client.tlsCerKeyPath = [[NSBundle mainBundle] pathForResource:@"client" ofType:@"key"];
client.tlsCafile = [[NSBundle mainBundle] pathForResource:@"server" ofType:@"crt"];
client.tlsPeerCertVerify = YES;
client.tlsInsecure = true;
    
[client connectWithCompletionHandler:^(MQTTConnectionReturnCode code) {
    if (code == 0)
    {
        NSLog(@"Connect Success");
    }
    else
    {
        NSLog(@"Connect Fail! State Code is %lu", (unsigned long)code);
    }
}];
```

## Authors

* [Jeff Mesnil](http://jmesnil.net/)

* [Noskthing](http://www.jianshu.com/u/463d70da11cb)

[mqtt]: http://public.dhe.ibm.com/software/dw/webservices/ws-mqtt/mqtt-v3r1.html
