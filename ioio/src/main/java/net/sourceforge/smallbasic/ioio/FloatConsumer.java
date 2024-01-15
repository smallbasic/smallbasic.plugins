package net.sourceforge.smallbasic.ioio;

import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.api.exception.IncompatibilityException;

@FunctionalInterface
public interface FloatConsumer<T>  {
  float invoke(T t) throws ConnectionLostException, InterruptedException, IncompatibilityException;
}