package net.ulno.libni.receiver;

/**
 * Created by ulno on 22.02.16.
 */
public class NetworkReceiverID {
    long sessionID; // random session ID
    long clientID; // self selected client id (by client)

    public NetworkReceiverID(long sessionID, long clientID) {
        this.sessionID = sessionID;
        this.clientID = clientID;
    }

    public NetworkReceiverID() {
        // empty
    }

    @Override
    public boolean equals(Object obj) {
        if(obj != null && obj instanceof NetworkReceiverID) {
            return ((NetworkReceiverID) obj).sessionID == sessionID && ((NetworkReceiverID) obj).clientID == clientID;
        }
        return false;
    }

    @Override
    public int hashCode() {
        // doesn't work in Android return Long.hashCode(sessionID+clientID);
        return (int)(sessionID+clientID); // can use the ids
    }

}
