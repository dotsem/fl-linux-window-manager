enum KeyboardMode {
  none(0),
  exclusive(1),
  onDemand(2);

  /// Integer representation of the enum. This should match with the int value
  /// of the platform side enum.
  final int value;

  const KeyboardMode(this.value);
}
