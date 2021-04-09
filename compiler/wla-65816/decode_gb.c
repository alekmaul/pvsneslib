
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

case 4:
case 1:
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
					fprintf(file_out_ptr, "d%d d%d ", opt_tmp->hex, d);
	      else if (z == INPUT_NUMBER_ADDRESS_LABEL)
					fprintf(file_out_ptr, "k%d d%d R%s ", active_file_info_last->line_current, opt_tmp->hex, label);
	      else {
					fprintf(file_out_ptr, "d%d c%d ", opt_tmp->hex, latest_stack);

					if (opt_tmp->type == 4) {
						/* 4 -> let's configure the stack so that all label references inside are relative */
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
