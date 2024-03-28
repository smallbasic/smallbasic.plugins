/*
 * Copyright 2011 Ytai Ben-Tsvi. All rights reserved.
 *
 *
 * Redistribution and use in source and binary forms, with or without modification, are
 * permitted provided that the following conditions are met:
 *
 *    1. Redistributions of source code must retain the above copyright notice, this list of
 *       conditions and the following disclaimer.
 *
 *    2. Redistributions in binary form must reproduce the above copyright notice, this list
 *       of conditions and the following disclaimer in the documentation and/or other materials
 *       provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL ARSHAN POURSOHI OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are those of the
 * authors and should not be interpreted as representing official policies, either expressed
 * or implied.
 */

package ioio.smallbasic.pc;

import com.fazecast.jSerialComm.SerialPort;

import java.util.Arrays;
import java.util.Collection;
import java.util.Collections;
import java.util.LinkedList;
import java.util.List;

import ioio.lib.api.IOIOConnection;
import ioio.lib.spi.IOIOConnectionBootstrap;
import ioio.lib.spi.IOIOConnectionFactory;
import ioio.lib.spi.Log;

public class SerialPortIOIOConnectionBootstrap implements IOIOConnectionBootstrap {
  private static final String TAG = "SerialPortIOIOConnectionBootstrap";

  @Override
  public void getFactories(Collection<IOIOConnectionFactory> result) {
    Collection<String> ports = getExplicitPorts();
    if (ports.isEmpty()) {
      Log.w(TAG, "ioio.SerialPorts not defined.\n"
            + "Will attempt to enumerate all possible ports (slow) "
            + "and connect to a IOIO over each one.\n"
            + "To fix, add the -Dioio.SerialPorts=xyz argument to "
            + "the java command line, where xyz is a colon-separated "
            + "list of port identifiers, e.g. COM1:COM2.");
      ports = getAvailablePorts();
    }
    for (final String port : ports) {
      Log.d(TAG, "Adding serial port " + port);
      result.add(new IOIOConnectionFactory() {
          @Override
          public IOIOConnection createConnection() {
            return new SerialPortIOIOConnection(port);
          }

          @Override
          public Object getExtra() {
            return port;
          }

          @Override
          public String getType() {
            return SerialPortIOIOConnection.class.getCanonicalName();
          }
        });
    }
  }

  static Collection<String> getAvailablePorts() {
    List<String> result = new LinkedList<>();
    for (SerialPort port : SerialPort.getCommPorts()) {
      if (port.openPort()) {
        Log.d(TAG, "Adding serial port " + port.getDescriptivePortName());
        result.add(port.getDescriptivePortName());
        port.closePort();
      }
    }
    return result;
  }

  static Collection<String> getExplicitPorts() {
    Collection<String> result;
    String property = System.getProperty("ioio.SerialPorts");
    if (property == null) {
      result = Collections.emptyList();
    } else {
      String[] portNames = property.split(":");
      result = new LinkedList<>(Arrays.asList(portNames));
    }
    return result;
  }
}
