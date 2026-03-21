)";

} // anonymous namespace

void
SetupShipsSchema (spacexpanse::SQLiteDatabase& db)
{
  /* Apply static SQL schema used by ships on-chain game state storage.  */
  LOG (INFO) << "Setting up the database schema for spacexpanseships...";
  db.Execute (SCHEMA_SQL);
}

} // namespace ships
