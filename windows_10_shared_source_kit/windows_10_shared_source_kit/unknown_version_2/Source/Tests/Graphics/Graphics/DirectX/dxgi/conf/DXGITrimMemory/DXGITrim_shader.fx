ARD when we see other
// flags like DEVICECREATE_PURE.  It is possible that
// the test has asked for both of those, in which case
// it's saying that it wants to be run first on one and
// then a second time on the other.  It could also
// request hardware and software vertex processing,
// meaning that it wants to run first with HWVP, then
// with SWVP.  This all means that we can really only
// mask out flags when we know for certain that the
// device doesn't support them, and thus there's no
// point in including them in the device mode list.
//
// And finally, DEVICECREATE_MIXED is an interesting
// character, because it can be paired with HWVP or
// SWVP.  We only ever mask it out if we determine that
// the device doesn't support HWVP, because that means
// that it doesn't support MIXED eith