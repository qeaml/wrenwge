# TODO

* Figure out why previous State doesn't get deleted when swapping states.
  * Note that popping a SubState *does* properly delete it.
  * In the sample, the `test.red` bind still properly works. Its existence
    implies the KeyBind never gets finalized. The fact pressing it doesn't crash
    further proves the `InitState` object still exists even after we switch to a
    different state.
