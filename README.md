# SmartLights

SmartLights is an intelligent IoT embedded system.  The system uses infrared motion detectors installed on an ESP8266 WiFi enabled microchip to determine when somebody enters a room.  Once motion is detected, a message is sent to a Master node (server) that processes the data and determines which lights should be turned on and off.

Instead of simply turning on a light when motion is detected, a server-in-the-middle approach allows for smarter decision making.  For example, a server will be able to make a decision on which lights should be turned on and for how long based on factors such as

- the structure of the house
- which sensors have recently detected motion
- number of occupants
- status of other lights/devices
- ect.

Naturally, this approach allows for more flexibility.  For example, while it may not be necessary for the initial release, SmartLights could eventually support a mobile and/or web portal that allows a user to manually control the status of certain lights.  If each node determined whether or not it should activate its lights independently of the rest, not only would such a change require a significant rewrite, but each node would have to be updated.

## Status

This project is still in active development; however, I am currently searching for alternative employment.  What that means is progress on the project will probably be a little slow as I will have to find time for it between work, life, job applications, and studying.  However, the project is still alive.

## Roadmap

Up to this point, I have tried to get some very basic infrastructure in place.  First, I developed the Scheduler to help "multithread" the Arduino architecture.  This is an extremely simplistic approach for multithreading and it lacks some major features, but it does add a layer of abstraction to performing multiple tasks at once (and increases the difficulty of the project, which was the goal).

Moving forward, here are the major milestones for this project:

- [ ] Build out the infrastructure library.  This is a collection of classes that will be generally useful for similar projects that use the architecture I set out for this project.
- [ ] Simple communication between the "Master node" (server) and the "Slave node".
- [ ] Finish sensor device/class
- [ ] Finish output device/class
- [ ] Build infrastructure for registering nodes/devices
- [ ] Build infrastructure for sending data from sensors
- [ ] Build infrastructure for receiving updates from the "Master node"
- [ ] Create web portal (see Connected Project for more details)
- [ ] Create mobile portal (see Connected Project for more details)

## Connected Projects

_Coming soon..._

Connected projects are those projects that are related to this project in some way, but have an independent repo.  For example, the server code and mobile/web portals will each have their own repos.