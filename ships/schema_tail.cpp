)";

} // anonymous namespace

void
SetupShipsSchema (spacexpanse::SQLiteDatabase& db)
{
  LOG (INFO) << "Setting up the database schema for spacexpanseships...";
  db.Execute (SCHEMA_SQL);
}

} // namespace ships
