# NetStack
Bare metal NetStack

## Notes
* Original design had one set of buffer shared by all network stacks/interfaces. This was great for multicast, but without scatter/gather in HW for separating headers from payload, the same buffer can't be sent to multiple stacks (headers like ethernet will be changed from under them).