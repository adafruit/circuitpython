import tomllib


def find_mpconfigboard(portdir, board_id):
    next_underscore = board_id.find("_")
    while next_underscore != -1:
        vendor = board_id[:next_underscore]
        board = board_id[next_underscore + 1 :]
        p = portdir / f"boards/{vendor}/{board}/circuitpython.toml"
        if p.exists():
            return p
        next_underscore = board_id.find("_", next_underscore + 1)
    return None


def load_mpconfigboard(portdir, board_id):
    mpconfigboard_path = find_mpconfigboard(portdir, board_id)
    if mpconfigboard_path is None or not mpconfigboard_path.exists():
        return None, {}

    with mpconfigboard_path.open("rb") as f:
        return mpconfigboard_path, tomllib.load(f)


def get_shields(mpconfigboard):
    shields = mpconfigboard.get("SHIELDS")
    if shields is None:
        shields = mpconfigboard.get("SHIELD")

    if shields is None:
        return []
    if isinstance(shields, str):
        return [shields]
    if isinstance(shields, (list, tuple)):
        return [str(shield) for shield in shields]

    return [str(shields)]
