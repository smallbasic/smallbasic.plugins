package ioio.smallbasic;

import ioio.lib.api.exception.ConnectionLostException;

@FunctionalInterface
public interface Function<T, I> {
  T apply(I i) throws ConnectionLostException, InterruptedException;
}
