
case 0:
for ( ; x < OP_SIZE_MAX; inz++, x++) {
	if (opt_tmp->op[x] == 0 && buffer[inz] == 0x0A) {
	  fprintf(file_out_ptr, "d%d ", opt_tmp->hex);
	  i = inz;
	  return SUCCEEDED;
	}
	if (opt_tmp->op[x] != toupper((int)buffer[inz]))
	  break;
 }
break;

case 11:
case 1:
for ( ; x < OP_SIZE_MAX; inz++, x++) {
	if (opt_tmp->op[x] == 'x') {
	  y = i;
	  i = inz;
	  z = input_number();
	  inz = i;
	  i = y;
	  if (!(z == SUCCEEDED || z == INPUT_NUMBER_ADDRESS_LABEL || z == INPUT_NUMBER_STACK))
	    return FAILED;
	  if (z == SUCCEEDED && (d > 255 || d < -128))
	    break;

	  for (x++ ; x < OP_SIZE_MAX; inz++, x++) {
	    if (opt_tmp->op[x] == 0 && buffer[inz] == 0x0A) {
	      if (z == SUCCEEDED)
					fprintf(file_out_ptr, "d%d d%d ", opt_tmp->hex, d);
	      else if (z == INPUT_NUMBER_ADDRESS_LABEL)
					fprintf(file_out_ptr, "k%d d%d R%s ", active_file_info_last->line_current, opt_tmp->hex, label);
	      else {
					fprintf(file_out_ptr, "d%d c%d ", opt_tmp->hex, latest_stack);
					if (opt_tmp->type == 11) {
						/* 11 -> let's configure the stack so that all label references inside are relative */
						stacks_tmp->relative_references = 1;
					}
				}

	      i = inz;
	      return SUCCEEDED;
	    }
	    if (opt_tmp->op[x] != toupper((int)buffer[inz]))
	      break;
	  }
	}
	if (opt_tmp->op[x] != toupper((int)buffer[inz]))
	  break;
 }
break;

case 2:
for ( ; x < OP_SIZE_MAX; inz++, x++) {
	if (opt_tmp->op[x] == '?') {
	  y = i;
	  i = inz;
	  z = input_number();
	  inz = i;
	  i = y;
	  if (!(z == SUCCEEDED || z == INPUT_NUMBER_ADDRESS_LABEL || z == INPUT_NUMBER_STACK))
	    return FAILED;
	  if (z == SUCCEEDED && (d > 65535 || d < -32768)) {
	    print_error("Out of 16bit range.\n", ERROR_NUM);
	    return FAILED;
	  }

	  for (x++ ; x < OP_SIZE_MAX; inz++, x++) {
	    if (opt_tmp->op[x] == 0 && buffer[inz] == 0x0A) {
	      if (z == SUCCEEDED)
					fprintf(file_out_ptr, "d%d y%d ", opt_tmp->hex, d);
	      else if (z == INPUT_NUMBER_ADDRESS_LABEL)
					fprintf(file_out_ptr, "k%d d%d r%s ", active_file_info_last->line_current, opt_tmp->hex, label);
	      else
					fprintf(file_out_ptr, "d%d C%d ", opt_tmp->hex, latest_stack);

	      i = inz;
	      return SUCCEEDED;
	    }
	    if (opt_tmp->op[x] != toupper((int)buffer[inz]))
	      break;
	  }
	}
	if (opt_tmp->op[x] != toupper((int)buffer[inz]))
	  break;
 }
break;

case 3:
for ( ; x < OP_SIZE_MAX; inz++, x++) {
	if (opt_tmp->op[x] == 0 && buffer[inz] == 0x0A) {
	  fprintf(file_out_ptr, "y%d ", opt_tmp->hex);
	  i = inz;
	  return SUCCEEDED;
	}
	if (opt_tmp->op[x] != toupper((int)buffer[inz]))
	  break;
 }
break;

case 4:
for ( ; x < OP_SIZE_MAX; inz++, x++) {
	if (opt_tmp->op[x] == 'x' || opt_tmp->op[x] == 's') {
	  y = i;
	  i = inz;
	  z = input_number();
	  inz = i;
	  i = y;
	  if (!(z == SUCCEEDED || z == INPUT_NUMBER_ADDRESS_LABEL || z == INPUT_NUMBER_STACK))
	    return FAILED;

		if (z == SUCCEEDED) {
			if ((opt_tmp->op[x] == 'x' && (d > 255 || d < -128)) ||
					(opt_tmp->op[x] == 's' && (d > 127 || d < -128))) {
				print_error("Out of 8bit range.\n", ERROR_NUM);
				return FAILED;
			}
		}

	  for (x++; x < OP_SIZE_MAX; inz++, x++) {
	    if (opt_tmp->op[x] == 0 && buffer[inz] == 0x0A) {
	      if (z == SUCCEEDED)
					fprintf(file_out_ptr, "y%d d%d ", opt_tmp->hex, d);
	      else if (z == INPUT_NUMBER_ADDRESS_LABEL)
					fprintf(file_out_ptr, "k%d y%d R%s ", active_file_info_last->line_current, opt_tmp->hex, label);
	      else
					fprintf(file_out_ptr, "y%d c%d ", opt_tmp->hex, latest_stack);

	      i = inz;
	      return SUCCEEDED;
	    }
	    if (opt_tmp->op[x] != toupper((int)buffer[inz]))
	      break;
	  }
	}
	if (opt_tmp->op[x] != toupper((int)buffer[inz]))
	  break;
 }
break;

case 5:
for ( ; x < OP_SIZE_MAX; inz++, x++) {
	if (opt_tmp->op[x] == 'x' || opt_tmp->op[x] == 's') {
	  y = i;
	  i = inz;
	  z = input_number();
	  inz = i;
	  i = y;
	  if (!(z == SUCCEEDED || z == INPUT_NUMBER_ADDRESS_LABEL || z == INPUT_NUMBER_STACK))
	    return FAILED;

		if (z == SUCCEEDED) {
			if ((opt_tmp->op[x] == 'x' && (d > 255 || d < -128)) ||
					(opt_tmp->op[x] == 's' && (d > 127 || d < -128))) {
				print_error("Out of 8bit range.\n", ERROR_NUM);
				return FAILED;
			}
		}

	  for (x++ ; x < OP_SIZE_MAX; inz++, x++) {
	    if (opt_tmp->op[x] == 0 && buffer[inz] == 0x0A) {
	      if (z == SUCCEEDED)
					fprintf(file_out_ptr, "y%d d%d d%d ", opt_tmp->hex, d, opt_tmp->hex_x);
	      else if (z == INPUT_NUMBER_ADDRESS_LABEL)
					fprintf(file_out_ptr, "k%d y%d R%s d%d ", active_file_info_last->line_current, opt_tmp->hex, label, opt_tmp->hex_x);
	      else
					fprintf(file_out_ptr, "y%d c%d d%d ", opt_tmp->hex, latest_stack, opt_tmp->hex_x);

	      i = inz;
	      return SUCCEEDED;
	    }
	    if (opt_tmp->op[x] != toupper((int)buffer[inz]))
	      break;
	  }
	}
	if (opt_tmp->op[x] != toupper((int)buffer[inz]))
	  break;
 }
break;

case 6:
for ( ; x < OP_SIZE_MAX; inz++, x++) {
	if (opt_tmp->op[x] == '?') {
	  y = i;
	  i = inz;
	  z = input_number();
	  inz = i;
	  i = y;
	  if (!(z == SUCCEEDED || z == INPUT_NUMBER_ADDRESS_LABEL || z == INPUT_NUMBER_STACK))
	    return FAILED;

	  for (x++ ; x < OP_SIZE_MAX; inz++, x++) {
	    if (opt_tmp->op[x] == 0 && buffer[inz] == 0x0A) {
	      if (z == SUCCEEDED)
					fprintf(file_out_ptr, "y%d y%d ", opt_tmp->hex, d);
	      else if (z == INPUT_NUMBER_ADDRESS_LABEL)
					fprintf(file_out_ptr, "k%d y%d r%s ", active_file_info_last->line_current, opt_tmp->hex, label);
	      else
					fprintf(file_out_ptr, "y%d C%d ", opt_tmp->hex, latest_stack);

	      i = inz;
	      return SUCCEEDED;
	    }
	    if (opt_tmp->op[x] != toupper((int)buffer[inz]))
	      break;
	  }
	}
	if (opt_tmp->op[x] != toupper((int)buffer[inz]))
	  break;
 }
break;

case 7:
for ( ; x < OP_SIZE_MAX; inz++, x++) {
	if (opt_tmp->op[x] == 'x' || opt_tmp->op[x] == 's') {
	  y = i;
	  i = inz;
	  z = input_number();
	  inz = i;
	  i = y;
	  if (!(z == SUCCEEDED || z == INPUT_NUMBER_ADDRESS_LABEL || z == INPUT_NUMBER_STACK))
	    return FAILED;

		if (z == SUCCEEDED) {
			if ((opt_tmp->op[x] == 'x' && (d > 255 || d < -128)) ||
					(opt_tmp->op[x] == 's' && (d > 127 || d < -128))) {
				print_error("Out of 8bit range.\n", ERROR_NUM);
				return FAILED;
			}
		}

	  e = d;
	  v = z;
	  h = latest_stack;
	  if (z == INPUT_NUMBER_ADDRESS_LABEL)
	    strcpy(labelx, label);

	  for (x++ ; x < OP_SIZE_MAX; inz++, x++) {
	    if (opt_tmp->op[x] == 'x') {
	      y = i;
	      i = inz;
	      z = input_number();
	      inz = i;
	      i = y;
	      if (!(z == SUCCEEDED || z == INPUT_NUMBER_ADDRESS_LABEL || z == INPUT_NUMBER_STACK))
					return FAILED;
	      if (z == SUCCEEDED && (d > 255 || d < -128))
					break;

	      for (x++ ; x < OP_SIZE_MAX; inz++, x++) {
					if (opt_tmp->op[x] == 0 && buffer[inz] == 0x0A) {
						if (v == SUCCEEDED)
							fprintf(file_out_ptr, "y%d d%d ", opt_tmp->hex, e);
						else if (v == INPUT_NUMBER_ADDRESS_LABEL)
							fprintf(file_out_ptr, "k%d y%d R%s ", active_file_info_last->line_current, opt_tmp->hex, labelx);
						else
							fprintf(file_out_ptr, "y%d c%d ", opt_tmp->hex, h);

						if (z == SUCCEEDED)
							fprintf(file_out_ptr, "d%d ", d);
						else if (z == INPUT_NUMBER_ADDRESS_LABEL)
							fprintf(file_out_ptr, "k%d R%s ", active_file_info_last->line_current, label);
						else
							fprintf(file_out_ptr, "c%d ", latest_stack);

						i = inz;
						return SUCCEEDED;
					}
					if (opt_tmp->op[x] != toupper((int)buffer[inz]))
						break;
	      }
	    }
	    if (opt_tmp->op[x] != toupper((int)buffer[inz]))
	      break;
	  }
	}
	if (opt_tmp->op[x] != toupper((int)buffer[inz]))
	  break;
 }
break;

case 8:
for ( ; x < OP_SIZE_MAX; inz++, x++) {
	if (opt_tmp->op[x] == '*') {
	  y = i;
	  i = inz;
	  z = input_number();
	  inz = i;
	  i = y;
	  if (z != SUCCEEDED || d != opt_tmp->value)
	    break;

	  for (x++ ; x < OP_SIZE_MAX; inz++, x++) {
	    if (opt_tmp->op[x] == 0 && buffer[inz] == 0x0A) {
	      fprintf(file_out_ptr, "d%d ", opt_tmp->hex);
	      i = inz;
	      return SUCCEEDED;
	    }
	    if (opt_tmp->op[x] != toupper((int)buffer[inz]))
	      break;
	  }
	}
	if (opt_tmp->op[x] != toupper((int)buffer[inz]))
	  break;
 }
break;

case 9:
for ( ; x < OP_SIZE_MAX; inz++, x++) {
	if (opt_tmp->op[x] == '*') {
	  y = i;
	  i = inz;
	  z = input_number();
	  inz = i;
	  i = y;
	  if (z != SUCCEEDED || d != opt_tmp->value)
	    break;

	  for (x++ ; x < OP_SIZE_MAX; inz++, x++) {
	    if (opt_tmp->op[x] == 0 && buffer[inz] == 0x0A) {
	      fprintf(file_out_ptr, "y%d ", opt_tmp->hex);
	      i = inz;
	      return SUCCEEDED;
	    }
	    if (opt_tmp->op[x] != toupper((int)buffer[inz]))
	      break;
	  }
	}
	if (opt_tmp->op[x] != toupper((int)buffer[inz]))
	  break;
 }
break;

case 10:
for ( ; x < OP_SIZE_MAX; inz++, x++) {
	if (opt_tmp->op[x] == '*') {
	  y = i;
	  i = inz;
	  z = input_number();
	  inz = i;
	  i = y;
	  if (z != SUCCEEDED || d != opt_tmp->value)
	    break;

	  for (x++ ; x < OP_SIZE_MAX; inz++, x++) {
	    if (opt_tmp->op[x] == 'x' || opt_tmp->op[x] == 's') {
	      y = i;
	      i = inz;
	      z = input_number();
	      inz = i;
	      i = y;
	      if (!(z == SUCCEEDED || z == INPUT_NUMBER_ADDRESS_LABEL || z == INPUT_NUMBER_STACK))
					return FAILED;

	      if (z == SUCCEEDED) {
					if ((opt_tmp->op[x] == 'x' && (d > 255 || d < -128)) ||
							(opt_tmp->op[x] == 's' && (d > 127 || d < -128))) {
						print_error("Out of 8bit range.\n", ERROR_NUM);
						return FAILED;
					}
	      }

	      for (x++ ; x < OP_SIZE_MAX; inz++, x++) {
					if (opt_tmp->op[x] == 0 && buffer[inz] == 0x0A) {
						if (z == SUCCEEDED)
							fprintf(file_out_ptr, "y%d d%d d%d ", opt_tmp->hex, d, opt_tmp->hex_x);
						else if (z == INPUT_NUMBER_ADDRESS_LABEL)
							fprintf(file_out_ptr, "k%d y%d R%s d%d ", active_file_info_last->line_current, opt_tmp->hex, label, opt_tmp->hex_x);
						else
							fprintf(file_out_ptr, "y%d c%d d%d ", opt_tmp->hex, latest_stack, opt_tmp->hex_x);

						i = inz;
						return SUCCEEDED;
					}
					if (opt_tmp->op[x] != toupper((int)buffer[inz]))
						break;
	      }
	    }
	    if (opt_tmp->op[x] != toupper((int)buffer[inz]))
	      break;
	  }
	}
	if (opt_tmp->op[x] != toupper((int)buffer[inz]))
	  break;
 }
break;
