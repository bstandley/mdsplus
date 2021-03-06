/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package jScope;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.InetAddress;
import java.net.Socket;
import java.net.UnknownHostException;

/**
 *
 * @author manduchi
 */
public class MdsConnectionUdt extends MdsConnection
{
   public void connectToServer() throws IOException
   {
        if(provider != null)
        {
            host = getProviderHost();
            port = getProviderPort();
            user = getProviderUser();

            MdsIpProtocolWrapper mipw = new MdsIpProtocolWrapper("udt://"+host+":"+port);
//            MdsIpProtocolWrapper mipw = new MdsIpProtocolWrapper("tcp://"+host+":"+port);
//            dis = new DataInputStream(new BufferedInputStream(mipw.getInputStream()));
            dis = mipw.getInputStream();
            dos = new DataOutputStream(mipw.getOutputStream());
        }
    }
}
