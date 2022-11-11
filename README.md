# embedded-loop-station
Definitions: Loop: a repeating musical figure. When we hit the end of a loop, it immediately starts playing again from the beginning Track: a unique place to record audio within a loop with level, pan controls Objectives Create an application for any available MCU Development Board, such as Arduino, Teensy, STM32F4-Discovery that satisfies the following requirements: 

Store and access an arbitrarily large number of loops 
Can play at least one loop at a time 
A loop can be comprised of at least 4 mono or two stereo tracks. (The maximum number of concurrent tracks in a loop may be limited by your processor choice).
Can playback and record at the same time. 
Immediately upon ending the record state, the recorded track plays back in time. 
Includes at least three of the following features: 

Time Stretching: change tempo without affecting pitch of loop
Pitch Shifting: change pitch or key of each track independently
Loop trimming: Auto-trim a track to match start and end points of a loop, tracking and matching the BPM in use
Reverse Play: set a track to play backward within a loop
Overdub: Sets record mode. When in Overdub recording will be mixed with any existing material on a given track. Replace mode over-writes the content of a selected track.
Slip: offset the start point of a given track or combination of selected tracks.
Pan: control relative signal left and right Consider user-interaction. An audio looper is designed for playback and recording to happen concurrently. What controls and features make real-time recording and playback of loops very easy and straightforward? For example, how do you present punch in and punch out of recording as a loop is playing?


