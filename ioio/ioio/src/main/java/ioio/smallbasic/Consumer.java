package ioio.smallbasic;

import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.api.exception.IncompatibilityException;

@FunctionalInterface
public interface Consumer<T>  {
  void accept(T t) throws ConnectionLostException, InterruptedException, IncompatibilityException;
}