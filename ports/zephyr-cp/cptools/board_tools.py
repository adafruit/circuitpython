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
