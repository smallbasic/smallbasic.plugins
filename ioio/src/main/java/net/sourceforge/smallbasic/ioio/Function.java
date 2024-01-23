package net.sourceforge.smallbasic.ioio;

import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.api.exception.IncompatibilityException;

@FunctionalInterface
public interface Function<T, I> {
  T apply(I i) throws ConnectionLostException, InterruptedException, IncompatibilityException;
}
